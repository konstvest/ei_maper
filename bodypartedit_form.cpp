#include "bodypartedit_form.h"
#include "ui_bodypartedit_form.h"
#include "layout_components/bodypart_checkbox.h"
#include <QSharedPointer>

CBodyPartEditForm::CBodyPartEditForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CBodyPartEditForm)
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

