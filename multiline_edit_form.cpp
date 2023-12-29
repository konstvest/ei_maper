#include "multiline_edit_form.h"
#include "ui_multiline_edit_form.h"

CMultiLineEditForm::CMultiLineEditForm(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CMultiLineEditForm)
{
    ui->setupUi(this);
}

CMultiLineEditForm::~CMultiLineEditForm()
{
    delete ui;
}

void CMultiLineEditForm::setText(const QString& text)
{
    ui->plainTextEdit->setPlainText(text); //text.join("\n")
}

void CMultiLineEditForm::on_pushApply_clicked()
{
    emit onTextApplySignal(ui->plainTextEdit->toPlainText());
    close();
}


void CMultiLineEditForm::on_pushCancel_clicked()
{
    close();
}

