#include <QMap>
#include <QString>
#include <QStringList>
#include <QKeyEvent>

#include "select_window.h"
#include "ui_select_window.h"
#include "view.h"
#include "scene.h"

CSelectForm::CSelectForm(QWidget *parent) :
    QWidget(parent)
    ,ui(new Ui::CSelectForm)
    ,m_pView(nullptr)
    ,m_selected_num(0)
{
    ui->setupUi(this);
    init();
}

void CSelectForm::onShow()
{
    show();
}

void CSelectForm::selectAll()
{
    SSelect sel{eSelectType_all, "", ""};
    if(CScene::getInstance()->getMode() == eEditModeLogic)
        sel.param1="logic";
    m_selected_num = m_pView->select(sel, false);
}

void CSelectForm::keyPressEvent(QKeyEvent *event)
{
    if (event->nativeVirtualKey() == eKey_Esc)
        close();
}

void CSelectForm::init()
{
    setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);

    m_loc["Id range"] = {eSelectType_Id_range, "Start Id", "End Id", true};//QStringList{, "Start Id", "End Id"};
    m_loc["Map name"] = {eSelectType_Map_name, "Map name", "", false};
    m_loc["Texture name"] = {eSelectType_Texture_name, "Texture name", "", false};
    m_loc["Model name"] = {eSelectType_Model_name, "Model name", "", false};
    //m_loc["Mob file"] = {eSelectType_Mob_file, "Mob name", ""};
    m_loc["Diplomacy group"] = {eSelectType_Diplomacy_group, "Start group number", "End group number", true};
    //TODO
    //m_loc["Position (circle)"] = {eSelectType_Position_circle, "Center point", "Radius"};
    //m_loc["Position (rectangle)"] = {eSelectType_Position_rectangle, "Left down corner", "Right up corner"};
    m_loc["Database name"] = {eSelectType_Database_name, "Database name", "", false};
    m_loc["Template"] = {eSelectType_Template, "Template", "", false};
    //m_loc["Object type"] = {eSelectType_ObjectType, "Object type", ""};

    for (const auto& pair : m_loc.toStdMap())
        ui->combo_SelType->addItem(pair.first);
}

void CSelectForm::on_push_Close_clicked()
{
    close();
}

void CSelectForm::updateLabels(const QString &type)
{
    ui->label_Param_1->setText(m_loc[type].param1);
    ui->lineEdit_Param_1->clear();
    if (m_loc[type].param2.isEmpty())
    {
        ui->label_Param_2->hide();
        ui->lineEdit_Param_2->hide();
    }
    else
    {
        ui->label_Param_2->show();
        ui->label_Param_2->setText(m_loc[type].param2);
        ui->lineEdit_Param_2->show();
        ui->lineEdit_Param_2->clear();
    }
    if(!m_loc[type].exactMatch)
    { //set visible checkbox only for non-exactMatch options by default
        ui->check_exactMatch->setVisible(true);
        ui->check_exactMatch->setChecked(false);
    }
    else
    {
        ui->check_exactMatch->setChecked(true);
        ui->check_exactMatch->setVisible(false);
    }
}

void CSelectForm::updateTotal()
{
    ui->label_total->setText("Selected objects count:"+ QString::number(m_selected_num));
}

void CSelectForm::on_combo_SelType_currentIndexChanged(const QString &arg1)
{
    updateLabels(arg1);
}

void CSelectForm::on_push_Select_clicked()
{
    SSelect sel{m_loc[ui->combo_SelType->currentText()].type, ui->lineEdit_Param_1->text(), ui->lineEdit_Param_2->text(), ui->check_exactMatch->isChecked()};
    m_selected_num = m_pView->select(sel, false);
    updateTotal();
}

void CSelectForm::on_push_Add_to_select_clicked()
{
    SSelect sel{m_loc[ui->combo_SelType->currentText()].type, ui->lineEdit_Param_1->text(), ui->lineEdit_Param_2->text(), ui->check_exactMatch->isChecked()};
    m_selected_num = m_pView->select(sel, true);
    updateTotal();
}
