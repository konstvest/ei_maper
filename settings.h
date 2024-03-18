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

///
/// \brief The CSettings class provides a UI for changing options.
///
class CSettings : public QWidget
{
    Q_OBJECT

public:
    explicit CSettings(QWidget *parent = nullptr);
    ~CSettings();

    COpt* opt(QString name);
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
    void on_figPathAdd_clicked();
    void on_texPathAdd_clicked();
    void on_figPathRemove_clicked();
    void on_texPathRemove_clicked();
    void on_texPathUp_clicked();
    void on_texpathDown_clicked();
    void on_figPathUp_clicked();
    void on_figPathDown_clicked();

private:
    void initOptions();
    void readOptions();
    void saveOptions();
    void updatePathOpt(const char* name, EOptSet optSet);
    void updateOptUi();
    void updateOptFromUi();

private:
    Ui::CSettings *ui;
    MainWindow* m_mainWindow;

    QFile m_fileOpt;
    QMap<EOptSet, QVector<QSharedPointer<COpt>>> m_aOptCategory;
    double m_version; //todo: decrease to float
};

#endif // SETTINGS_H
