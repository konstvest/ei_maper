#ifndef MULTILINE_EDIT_FORM_H
#define MULTILINE_EDIT_FORM_H

#include <QDialog>

namespace Ui {
class CMultiLineEditForm;
}

///
/// \brief The CMultiLineEditForm class provides a form for editing multi-line parameters. For example, quest items, equipment.
///
class CMultiLineEditForm : public QDialog
{
    Q_OBJECT

public:
    explicit CMultiLineEditForm(QWidget *parent = nullptr);
    ~CMultiLineEditForm();
    void setText(const QString& text);

signals:
    void onTextApplySignal(QString);

private slots:
    void on_pushApply_clicked();
    void on_pushCancel_clicked();

private:
    Ui::CMultiLineEditForm *ui;
};

#endif // MULTILINE_EDIT_FORM_H
