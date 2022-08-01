#include <QMap>
#include <QString>
#include <QStringList>
#include <QKeyEvent>

#include "select_window.h"
#include "ui_selector.h"
#include "view.h"

CSelector::CSelector(QWidget *parent) :
    QWidget(parent)
    ,ui(new Ui::CSelector)
    ,m_pMainWindow(nullptr)
    ,m_pView(nullptr)
    ,m_selected_num(0)
{
    ui->setupUi(this);
    init();
}

void CSelector::onShow()
{
    show();
}

void CSelector::selectAll()
{
    SSelect sel{eSelectType_all, "", ""};
    m_selected_num = m_pView->select(sel, false);
}

void CSelector::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape)
        close();
}

void CSelector::init()
{
    setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);

    m_loc["Id range"] = {eSelectType_Id_range, "Start Id", "End Id"};//QStringList{, "Start Id", "End Id"};
    m_loc["Map name"] = {eSelectType_Map_name, "Map name", ""};
    m_loc["Texture name"] = {eSelectType_Texture_name, "Texture name", ""};
    m_loc["Model name"] = {eSelectType_Model_name, "Model name", ""};
    //m_loc["Mob file"] = {eSelectType_Mob_file, "Mob name", ""};
    m_loc["Diplomacy group"] = {eSelectType_Diplomacy_group, "Start group number", "End group number"};
    //TODO
    //m_loc["Position (circle)"] = {eSelectType_Position_circle, "Center point", "Radius"};
    //m_loc["Position (rectangle)"] = {eSelectType_Position_rectangle, "Left down corner", "Right up corner"};
    m_loc["Database name"] = {eSelectType_Database_name, "Database name", ""};
    m_loc["Template"] = {eSelectType_Template, "Template", ""};
    //m_loc["Object type"] = {eSelectType_ObjectType, "Object type", ""};

    for (const auto& pair : m_loc.toStdMap())
        ui->combo_SelType->addItem(pair.first);
}

void CSelector::on_push_Close_clicked()
{
    close();
}

void CSelector::updateLabels(const QString &type)
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
}

void CSelector::updateTotal()
{
    ui->label_total->setText("Selected objects count:"+ QString::number(m_selected_num));
}

void CSelector::on_combo_SelType_currentIndexChanged(const QString &arg1)
{
    updateLabels(arg1);
}

void CSelector::on_push_Select_clicked()
{
    SSelect sel{m_loc[ui->combo_SelType->currentText()].type, ui->lineEdit_Param_1->text(), ui->lineEdit_Param_2->text()};
    m_selected_num = m_pView->select(sel, false);
    updateTotal();
}

void CSelector::on_push_Add_to_select_clicked()
{
    SSelect sel{m_loc[ui->combo_SelType->currentText()].type, ui->lineEdit_Param_1->text(), ui->lineEdit_Param_2->text()};
    m_selected_num = m_pView->select(sel, true);
    updateTotal();
}
