#ifndef SETTINGS_H
#define SETTINGS_H

#include <QWidget>
#include <QLabel>
#include <QFile>
#include <QSharedPointer>
#include "options.h"

namespace Ui {
class CSettings;
}

enum EOptSet
{
    eOptSetGeneral = 0
    ,eOptSetRender
    ,eOptSetKeyBinding
    ,eOptSetResource
    ,eOptSetCount
};
class MainWindow;

class CSettings : public QWidget
{
    Q_OBJECT

public:
    explicit CSettings(QWidget *parent = nullptr);
    ~CSettings();

    COpt* opt(EOptSet optSet, QString& name);
    COpt* opt(EOptSet optSet, const char* name);
    void onShow(EOptSet optSet = eOptSetGeneral);
    bool onClose();
    void resetOptions();
    void attachMainWindow(MainWindow* mainWindow) {m_mainWindow = mainWindow;}

protected:
    void closeEvent(QCloseEvent* e);

public Q_SLOTS:
    bool close();

private slots:
    void on_buttonApply_clicked();
    void on_buttonCancel_clicked();
    void on_FigurePath_1_open_clicked();
    void on_FigurePath_2_open_clicked();
    void on_TexturePath_1_open_clicked();
    void on_TexturePath_2_open_clicked();

private:
    void initOptions();
    void readOptions();
    void saveOptions();
    void updatePathOpt(const char* name, EOptSet optSet);

private:
    Ui::CSettings *ui;
    MainWindow* m_mainWindow;

    QFile m_fileOpt;
    double m_version;
    QVector<QVector<QSharedPointer<COpt>>> m_aOptCategory;

};

#endif // SETTINGS_H
