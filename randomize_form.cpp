#include "randomize_form.h"
#include "ui_randomize_form.h"
#include "view.h"
#include "property.h"

CRandomizeForm::CRandomizeForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CRandomizeForm)
{
    ui->setupUi(this);
    init();
}

CRandomizeForm::~CRandomizeForm()
{
    delete ui;
}

void CRandomizeForm::init()
{
    setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
    setWindowTitle("Randomize parameters");

    m_typeMap["Complection Z"] = eObjParam_COMPLECTION_Z;
    m_typeMap["Complection X"] = eObjParam_COMPLECTION_X;
    m_typeMap["Complection Y"] = eObjParam_COMPLECTION_Y;
    m_typeMap["Rotation X"] = eObjParam_ROTATION_X;
    m_typeMap["Rotation Y"] = eObjParam_ROTATION_Y;
    m_typeMap["Rotation Z"] = eObjParam_ROTATION_Z;
    m_typeMap["Position X"] = eObjParam_POSITION_X;
    m_typeMap["Position Y"] = eObjParam_POSITION_Y;
    m_typeMap["Position Z"] = eObjParam_POSITION_Z;
    //textures? models?
    ui->combo_setType->addItems(m_typeMap.keys());
    ui->combo_setType->setCurrentText(m_typeMap.key(eObjParam_COMPLECTION_Z, "undefined"));
    propFloat fl(eObjParam_COMPLECTION_Z);
    ui->lineEdit_Min->setText("0");
    ui->lineEdit_Max->setText("1");
    ui->lineEdit_Min->setValidator(new QRegExpValidator(fl.validationRegExp(), ui->lineEdit_Min)); //dont work correctly if re-use validator
    ui->lineEdit_Max->setValidator(new QRegExpValidator(fl.validationRegExp(), ui->lineEdit_Max));
}

void CRandomizeForm::on_push_Randomize_clicked()
{
    m_pView->setRandomComplection(m_typeMap[ui->combo_setType->currentText()], ui->lineEdit_Min->text().toFloat(), ui->lineEdit_Max->text().toFloat());
}


void CRandomizeForm::on_push_Close_clicked()
{
    close();
}

