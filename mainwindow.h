#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QKeyEvent>
#include <QSharedPointer>

namespace Ui {
class MainWindow;
}

class CSettings;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

protected:
    void keyPressEvent(QKeyEvent* event);

private slots:
//    void on_xRotSlider_valueChanged(int value);
//    void on_yRotSlider_valueChanged(int value);
//    void on_zRotSlider_valueChanged(int value);

//    void on_pushButton_clicked();

//    void on_xMoveSlider_valueChanged(int value);

//    void on_yMoveSlider_valueChanged(int value);

//    void on_zMoveSlider_valueChanged(int value);

//    void on_pushButton_2_clicked();

//    void on_pushButton_3_clicked();

//    void on_pushButton_4_clicked();

//    void on_pushButton_5_clicked();

    void on_actionExit_triggered();

    void on_actionOpen_triggered();

    void on_pushButton_clicked();

    void on_actionSettings_triggered();

    void on_actionSave_as_triggered();

private:
    Ui::MainWindow* m_ui;
    QSharedPointer<CSettings> m_settings;
};

#endif // MAINWINDOW_H
