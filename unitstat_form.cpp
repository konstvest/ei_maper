#include "unitstat_form.h"
#include "ui_unitstat_form.h"
#include "resourcemanager.h"
#include "layout_components/tablemanager.h"

CUnitStatForm::CUnitStatForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CUnitStatForm)
{
    ui->setupUi(this);
    setWindowTitle("Unit stats");
    ui->tableUnitStat->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
}

CUnitStatForm::~CUnitStatForm()
{
    delete ui;
}

void CUnitStatForm::setStat(const QSharedPointer<IPropertyBase> &prop)
{
    reset();
    m_prop.reset(prop->clone());
    QMap<uint, QString> map;
    auto arrStat = dynamic_cast<propUnitStat*>(m_prop.get())->value();
    CResourceStringList::getInstance()->getPropList(map, eObjParam_UNIT_STATS);
    for(int i(0); i<51; ++i)
    {
        ui->tableUnitStat->insertRow(i);
        ui->tableUnitStat->setItem(i, 0, new QTableWidgetItem(map[i]));
        ui->tableUnitStat->item(i, 0)->setFlags(ui->tableUnitStat->item(i, 0)->flags() & ~Qt::ItemIsEditable);
        CValueItem* pCombo = new CValueItem(arrStat[i]);
        ui->tableUnitStat->setCellWidget(i, 1, pCombo);
    }
}

void CUnitStatForm::reset()
{
    ui->tableUnitStat->clear();
    ui->tableUnitStat->setColumnCount(2);
    ui->tableUnitStat->setRowCount(0);
    QStringList header;
    header.append("Parameter");
    header.append("Value");
    ui->tableUnitStat->setHorizontalHeaderLabels(header);
    ui->tableUnitStat->horizontalHeader()->setVisible(true);
    ui->tableUnitStat->verticalHeader()->hide();
}

void CUnitStatForm::on_pushCancel_clicked()
{
    close();
}


void CUnitStatForm::on_pushApply_clicked()
{
    auto* propStat = dynamic_cast<propUnitStat*>(m_prop.get());
    QVector<QSharedPointer<IPropertyBase>> arrStat = propStat->value();
    for(int i(0); i<51; ++i)
    {
        auto pValue = dynamic_cast<CValueItem*>(ui->tableUnitStat->cellWidget(i, 1));
        if(pValue->value()->toString().isEmpty())
            continue;

        arrStat[i].reset(pValue->value()->clone());
    }
    propStat->resetValue(arrStat);
    emit onApplyChangesSignal(m_prop);
}

