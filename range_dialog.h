#ifndef RANGE_DIALOG_H
#define RANGE_DIALOG_H

#include <QDialog>

namespace Ui {
class CRangeDialog;
}

class CRangeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CRangeDialog(QWidget *parent = nullptr);
    ~CRangeDialog();
    //void done(int res) override;

    void setRanges(uint min, uint max);

private:
    bool isValidValue();

private slots:
    void on_pushButtonCancel_clicked();

    void on_pushButtonApply_clicked();

private:
    Ui::CRangeDialog *ui;
};

#endif // RANGE_DIALOG_H
