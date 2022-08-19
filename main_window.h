#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QKeyEvent>
#include <QSharedPointer>
#include <QUndoStack>
#include <QUndoView>
#include <QPlainTextEdit>
#include "types.h"

namespace Ui {
class MainWindow;
}

class CSettings;
class CSelector;
class CCreateObjectForm;
class CView;

struct SWindowTitle
{
    QString mpr;
    QString activeMob;
    bool durty{false};
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private:
    void createUndoView();
    void initShortcuts();
    void connectUiButtons();
    bool isExitAllowed();

private slots:
    void on_actionExit_triggered();
    void on_actionOpen_triggered();
    void on_actionSettings_triggered();
    void on_actionSave_as_triggered();
    void on_actionSelect_All_triggered();
    void on_actionSelect_by_triggered();
    void on_actionShow_undo_redo_triggered();
    void on_actionClose_all_triggered();
    void on_actionSave_triggered();
    void on_action_Mob_parameters_triggered();
    void on_actionUndo_triggered();
    void on_toolButton_2_clicked();
    void on_selectButton_clicked();
    void on_moveButton_clicked();
    void on_actionRedo_triggered();
    void on_actionCreate_new_object_triggered();
    void updateWindowTitle(eTitleTypeData type, QString data);
    void on_actionSave_all_MOB_s_triggered();
    void on_rotateButton_clicked();
    void on_scaleButton_clicked();
    void on_actionReset_cam_position_triggered();
    void on_actionChange_mod_e_triggered();
    void on_actionCopy_IDs_to_clipboard_triggered();
    void on_actionSwitch_active_MOB_triggered();
    void on_actionOpen_2_triggered();

protected:
    void closeEvent(QCloseEvent* e);

private:
    Ui::MainWindow* m_ui;
    QSharedPointer<CSettings> m_settings;
    QSharedPointer<CSelector> m_selector;
    QSharedPointer<CCreateObjectForm> m_createDialog;
    QUndoStack* m_undoStack;
    QUndoView* m_undoView;
    CView* m_pView;
    SWindowTitle m_sWindowTitle;
};

#endif // MAIN_WINDOW_H
