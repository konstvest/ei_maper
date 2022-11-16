#include "round_mob_form.h"
#include "ui_round_mob_form.h"

CRoundMobForm::CRoundMobForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CRoundMobForm)
{
    setAttribute(Qt::WA_ShowWithoutActivating);
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    ui->setupUi(this);
}

CRoundMobForm::~CRoundMobForm()
{
    delete ui;
}

void CRoundMobForm::initMobList(QList<QString> &arrMob)
{
    for(auto& mobName : arrMob)
    {
        ui->mobList->addItem(mobName);
    }
    ui->mobList->setCurrentRow(0);
}

void CRoundMobForm::round()
{
    int row = ui->mobList->currentRow();
    if(row < 0)
        ui->mobList->setCurrentRow(0);
    else if(row == ui->mobList->count()-1)
        ui->mobList->setCurrentRow(0);
    else
        ui->mobList->setCurrentRow(row+1);
}

QString CRoundMobForm::selectedMob()
{
    return ui->mobList->currentItem()->text();
}
