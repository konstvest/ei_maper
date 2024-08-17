#include "bodypartedit_form.h"
#include "ui_bodypartedit_form.h"
#include "layout_components/bodypart_checkbox.h"
#include <QSharedPointer>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDir>

CBodyPartEditForm::CBodyPartEditForm(QWidget *parent) :
    QWidget(parent)
  ,ui(new Ui::CBodyPartEditForm)
  ,m_clipboardFile(QString("%1%2%3").arg(QDir::tempPath()).arg(QDir::separator()).arg("bodypart_buffer.json"))
{
    ui->setupUi(this);
    setWindowTitle("Bodypart visibility");
}

CBodyPartEditForm::~CBodyPartEditForm()
{
    delete ui;
}

void CBodyPartEditForm::setPartData(const QSharedPointer<IPropertyBase>& prop)
{
    reset();
    m_prop.reset(prop->clone());
    auto arrStat = dynamic_cast<propBodyPart*>(m_prop.get())->value();
    int i(0);
    for(auto& item: arrStat.toStdMap())
    {
        ui->tablePart->insertRow(i);
        ui->tablePart->setItem(i, 0, new QTableWidgetItem(item.first));
        auto pCheck = new CBodyPartCheckBox(ui->tablePart);
        auto state = Qt::CheckState::PartiallyChecked;
        if(item.second->isInit())
            state = item.second->value() ? Qt::CheckState::Checked : Qt::CheckState::Unchecked;
        pCheck->setCheckState(state);
        ui->tablePart->setCellWidget(i, 1, pCheck);
        ++i;
    }
}

void CBodyPartEditForm::reset()
{
    ui->tablePart->clear();
    ui->tablePart->setColumnCount(2);
    ui->tablePart->setRowCount(0);
    QStringList header;
    header.append("Part");
    header.append("");
    ui->tablePart->setHorizontalHeaderLabels(header);
    ui->tablePart->horizontalHeader()->setVisible(true);
    ui->tablePart->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui->tablePart->setColumnWidth(1, 8);
    ui->tablePart->verticalHeader()->hide();
}

void CBodyPartEditForm::on_pushCancel_clicked()
{
    close();
}


void CBodyPartEditForm::on_pushApply_clicked()
{
    auto arrStat = dynamic_cast<propBodyPart*>(m_prop.get())->value();

    for(int i(0); i<ui->tablePart->rowCount(); ++i)
    {
        //auto pBox = dynamic_cast<CBodyPartCheckBox*>(ui->tablePart->cellWidget(1, i));
        auto pBox = dynamic_cast<QCheckBox*>(ui->tablePart->cellWidget(i, 1));
        if(pBox->checkState() == Qt::CheckState::PartiallyChecked)
            continue;

        //arrStat[ui->tablePart->item(0,i)->text()] = QSharedPointer<propBool>(new propBool(eObjParam_BODYPARTS, pBox->checkState() == Qt::CheckState::Checked));
        arrStat[ui->tablePart->item(i,0)->text()]->resetValue(pBox->checkState() == Qt::CheckState::Checked);
    }
    emit onApplyChangesSignal(m_prop);
    close();
}


void CBodyPartEditForm::on_toolCopy_clicked()
{
    auto arrStat = dynamic_cast<propBodyPart*>(m_prop.get())->value();
    QJsonObject obj;
    obj.insert("Version", 2);

    QJsonObject part;
    for(int i(0); i<ui->tablePart->rowCount(); ++i)
    {
        auto pBox = dynamic_cast<QCheckBox*>(ui->tablePart->cellWidget(i, 1));
        if(pBox->checkState() == Qt::CheckState::PartiallyChecked)
            continue;

        part.insert(ui->tablePart->item(i,0)->text(), pBox->checkState() == Qt::CheckState::Checked);
    }
    obj.insert("bodyparts", part);
    QJsonDocument doc(obj);

    if (!m_clipboardFile.open(QIODevice::WriteOnly))
    {
        Q_ASSERT("Couldn't open bodypart copypast buffer file." && false);
    }
    else
    {
        m_clipboardFile.write(doc.toJson(QJsonDocument::JsonFormat::Indented));
        m_clipboardFile.close();
    }
}


void CBodyPartEditForm::on_toolPaste_clicked()
{
    if (!m_clipboardFile.open(QIODevice::ReadOnly))
    {
        Q_ASSERT("Couldn't open bodypart copypaste buffer file." && false);
        return;
    }

    if (m_clipboardFile.size() == 0)
    {
        qDebug() << "empty copypasteBuffer file";
        return;
    }

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(m_clipboardFile.readAll(), &parseError);
    m_clipboardFile.close();
    //todo: check if has no error
    QJsonObject obj = doc.object();
    if (obj["Version"].toInt() != 2)
        return;

    auto arrPart = obj["bodyparts"].toObject();
    if(arrPart.isEmpty())
    {
        ei::log(eLogInfo, "bodypart empty");
        return;
    }
    QStringList arrKey = arrPart.keys();
    QString partName;
    for(int i(0); i<ui->tablePart->rowCount(); ++i)
    {
        partName = ui->tablePart->item(i,0)->text();
        if(!arrKey.contains(partName))
            continue;

        auto pBox = dynamic_cast<QCheckBox*>(ui->tablePart->cellWidget(i, 1));
        pBox->setChecked(arrPart[partName].toBool()? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
    }

}

