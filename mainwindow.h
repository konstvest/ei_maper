#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QKeyEvent>
#include <QSharedPointer>
#include <QUndoStack>
#include <QUndoView>
#include <QPlainTextEdit>

namespace Ui {
class MainWindow;
}

class CSettings;
class CSelector;
class CMobParameters;
class CCreateObjectForm;

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
    void updateMobListInParam(bool bReset);
    void on_actionUndo_triggered();
    void on_toolButton_2_clicked();
    void on_selectButton_clicked();

    void on_moveButton_clicked();

private:
    Ui::MainWindow* m_ui;
    QSharedPointer<CSettings> m_settings;
    QSharedPointer<CSelector> m_selector;
    QSharedPointer<CMobParameters> m_mobParams;
    QSharedPointer<CCreateObjectForm> m_createDialog;
    QUndoStack* m_undoStack;
    QUndoView* m_undoView;

};

#endif // MAINWINDOW_H
