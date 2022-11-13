#ifndef CSELECTOR_H
#define CSELECTOR_H

#include <QWidget>
#include <QMap>
#include <QString>
#include <QStringList>

class CView;
class MainWindow;

namespace Ui {
class CSelector;
}

enum ESelectType
{
    eSelectType_Id_range
    ,eSelectType_Map_name
    ,eSelectType_Texture_name
    ,eSelectType_Model_name
    //,eSelectType_Mob_file
    ,eSelectType_Position_circle
    ,eSelectType_Position_rectangle
    ,eSelectType_Diplomacy_group
    ,eSelectType_Database_name
    ,eSelectType_Template
    ,eSelectType_ObjectType
    ,eSelectType_all
};

struct SSelect
{
    ESelectType type;
    //todo: param0 - object filters for find in ENodeType types
    QString param1;
    QString param2;
};

class CSelector : public QWidget
{
    Q_OBJECT
public:
    explicit CSelector(QWidget *parent = nullptr);
    void attachParents(MainWindow* pWindow, CView* pView){m_pMainWindow = pWindow;m_pView = pView;}
    void onShow();
    void selectAll();

protected:
    void keyPressEvent(QKeyEvent* event);

private:
    void init();
    void updateLabels(const QString& type);
    void updateTotal();

signals:

private slots:
    void on_push_Close_clicked();
    void on_combo_SelType_currentIndexChanged(const QString &arg1);
    void on_push_Select_clicked();
    void on_push_Add_to_select_clicked();

private:
    Ui::CSelector *ui;
    MainWindow* m_pMainWindow;
    CView* m_pView;
    QMap<QString, SSelect> m_loc;
    int m_selected_num;
};

#endif // CSELECTOR_H
