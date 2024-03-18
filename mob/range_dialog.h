#ifndef RANGE_DIALOG_H
#define RANGE_DIALOG_H

#include <QDialog>
#include "types.h"

namespace Ui {
class CRangeDialog;
}

class CMobParameters;
///
/// \brief The CRangeDialog class provides edit range parameter for *.mob file
///
class CRangeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CRangeDialog(QWidget *parent = nullptr);
    ~CRangeDialog();

    void initRanges(const QVector<SRange>& ranges, int currentIndex);

private:
    bool isValidValue();
    bool hasIntersection();

private slots:
    void on_pushButtonCancel_clicked();

    void on_pushButtonApply_clicked();

private:
    Ui::CRangeDialog *ui;
    CMobParameters* m_pParentWidget;
    QVector<SRange> m_arrRange;
    int m_index;
};

#endif // RANGE_DIALOG_H
