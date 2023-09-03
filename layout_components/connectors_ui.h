#ifndef CSTATUSHELPER_H
#define CSTATUSHELPER_H

#include <QString>
#include <QLabel>
#include <QLineEdit>

#include "types.h"

class CView;

class CStatusConnector
{
public:
    static CStatusConnector* getInstance();
    void attach(QLabel* pLabel, QLineEdit* pLine);
    void updateStatus(QString icoName, QString statusString);
private:
    CStatusConnector();
    ~CStatusConnector();

private:
    static CStatusConnector* m_statusHelper;
    QLabel* m_pIco;
    QLineEdit* m_pStatus;
};

class CButtonConnector
{
public:
    static CButtonConnector* getInstance();
    void attach(CView* pView);
    void addButton(EButtonOp btnType, QToolButton* pButtom);
    void pressButton(EButtonOp btnType);
    void clickButton(EButtonOp btnType);
private:
    CButtonConnector();
    ~CButtonConnector();

private:
    static CButtonConnector* m_buttonHelper;
    CView* m_pView;
    QMap<EButtonOp, QToolButton*> m_aButton;
};

class CIconManager
{
public:
    static CIconManager* getInstance();
    ~CIconManager(); //todo: call destructor, delete icon memory
    void updateStatus(QString icoName, QString statusString);
    void init();
    QIcon* icon(QString name);
private:
    CIconManager();

private:
    static CIconManager* m_resourceManager;
    QMap<QString, QIcon*> m_aIcon;
    //todo: localization strings
};
namespace ei {
    QIcon* icon(QString name);
    //todo: const QString& string(uint stringId);

}
#endif // CSTATUSHELPER_H
