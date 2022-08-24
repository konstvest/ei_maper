#include "range_dialog.h"
#include "ui_range_dialog.h"
#include <QCloseEvent>
#include <QIntValidator>
#include <QMessageBox>

CRangeDialog::CRangeDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CRangeDialog)
{
    ui->setupUi(this);
    setWindowTitle("Range changes dialog");
    setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
    // remove question mark from the title bar
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    uint maxvalue = 2147483647;
    ui->lineEditFrom->setValidator( new QIntValidator(0, maxvalue, this) ); //todo: create custom QUINT validator
    ui->lineEditTo->setValidator( new QIntValidator(0, maxvalue, this) );
}

CRangeDialog::~CRangeDialog()
{
    delete ui;
}

void CRangeDialog::setRanges(uint min, uint max)
{
    ui->lineEditFrom->setText(QString::number(min));
    ui->lineEditTo->setText(QString::number(max));
}

bool CRangeDialog::isValidValue()
{
    uint min = ui->lineEditFrom->text().toUInt();
    uint max = ui->lineEditTo->text().toUInt();
    bool bRes = (min > 0
                 && max > 0
                 && max > min
                 );
    if(!bRes)
    {
        QMessageBox::warning(this, "Warning", "Max and min ranges must not be 0 and max must be more then min");
    }
    return bRes;
}


void CRangeDialog::on_pushButtonCancel_clicked()
{
    done(QDialog::Rejected);
}


void CRangeDialog::on_pushButtonApply_clicked()
{
    if(isValidValue())
    {
        //TODO: check parent existing id's
        done(QDialog::Accepted);
    }
}

