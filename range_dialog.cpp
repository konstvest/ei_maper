#include "range_dialog.h"
#include "ui_range_dialog.h"
#include "mob_parameters.h"
#include <QCloseEvent>
#include <QIntValidator>
#include <QMessageBox>

CRangeDialog::CRangeDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CRangeDialog)
{
    ui->setupUi(this);
    m_pParentWidget = dynamic_cast<CMobParameters*>(parent);
    Q_ASSERT(m_pParentWidget);
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

void CRangeDialog::initRanges(const QVector<SRange> &ranges, int currentIndex)
{
    m_index = currentIndex;
    m_arrRange = ranges;

    if(currentIndex >= ranges.count())
    {//add new
        uint max = 1;
        SRange range;
        foreach(range, m_arrRange)
        {
            if(range.maxRange > max)
                max = range.maxRange+1;
        }
        ui->lineEditFrom->setText(QString::number(max));
        ui->lineEditTo->setText(QString::number(max+1000));
    }
    else
    {
        ui->lineEditFrom->setText(QString::number(ranges[m_index].minRange));
        ui->lineEditTo->setText(QString::number(ranges[m_index].maxRange));
    }

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

bool CRangeDialog::hasIntersection()
{
    uint min = ui->lineEditFrom->text().toUInt();
    uint max = ui->lineEditTo->text().toUInt();

    for(int i(0); i<m_arrRange.count(); ++i)
    {
        if(m_index == i)
            continue; //skip current index

        if((min >= m_arrRange[i].minRange && min <= m_arrRange[i].maxRange)
            || (max >= m_arrRange[i].minRange && max <= m_arrRange[i].maxRange))
        {
            QMessageBox::warning(this, "Warning", "Has intersection with range: " + QString::number(m_arrRange[i].minRange) + " - " + QString::number(m_arrRange[i].maxRange));
            return true;
        }
    }
    return false;
}


void CRangeDialog::on_pushButtonCancel_clicked()
{
    done(QDialog::Rejected);
}


void CRangeDialog::on_pushButtonApply_clicked()
{
    if(isValidValue() && !hasIntersection())
    {
        SRange range(ui->lineEditFrom->text().toUInt(), ui->lineEditTo->text().toUInt());
        m_pParentWidget->setNewRange(range, m_index);
        done(QDialog::Accepted);
    }
}

