#include <QIcon>
#include <QToolButton>
#include <QFile>
#include <QDebug>
#include <QDir>

#include "ui_connectors.h"
#include "log.h"
#include "view.h"
#include "utils.h"

CResourceManager* CResourceManager::m_resourceManager = nullptr;
CStatusConnector* CStatusConnector::m_statusHelper = nullptr;
CButtonConnector* CButtonConnector::m_buttonHelper = nullptr;

CStatusConnector* CStatusConnector::getInstance()
{
    if(nullptr == m_statusHelper)
        m_statusHelper = new CStatusConnector();
    return m_statusHelper;
}

void CStatusConnector::attach(QLabel *pLabel, QLineEdit *pLine)
{
    m_pIco = pLabel;
    m_pStatus = pLine;
    m_pStatus->setStyleSheet("* { background-color: rgba(0, 0, 0, 0); }"); //disable background
}

void CStatusConnector::updateStatus(QString icoName, QString statusString)
{
    if(!m_pStatus || !m_pIco)
        return;
    QIcon* pIcon = ei::icon(icoName);
    if(pIcon)
        m_pIco->setPixmap(pIcon->pixmap(12,12));

    m_pStatus->clear();
    m_pStatus->setText(statusString);
}

CStatusConnector::CStatusConnector()
    :m_pIco(nullptr)
    ,m_pStatus(nullptr)
{
}

CButtonConnector* CButtonConnector::getInstance()
{
    if(nullptr == m_buttonHelper)
        m_buttonHelper = new CButtonConnector();
    return m_buttonHelper;
}

void CButtonConnector::attach(CView *pView)
{
    m_pView = pView;
}

void CButtonConnector::addButton(EButtonOp btnType, QToolButton *pButton)
{
    m_aButton[btnType] = pButton;
    pButton->setIconSize(QSize(32, 32));
    switch (btnType) {
    case EButtonOpSelect:
        pButton->setIcon(*(ei::icon("select.ico"))); break;
    case EButtonOpMove:
        pButton->setIcon(*(ei::icon("move.ico"))); break;
    case EButtonOpRotate:
        pButton->setIcon(*(ei::icon("rotate.ico"))); break;
    case EButtonOpScale:
        pButton->setIcon(*(ei::icon("scale.ico"))); break;
    }
}

void CButtonConnector::pressButton(EButtonOp btnType)
{
    for(auto& pair : m_aButton.toStdMap())
    {
        pair.second->setDown(pair.first == btnType);
    }
}

void CButtonConnector::clickButton(EButtonOp btnType)
{
    for(auto& pair : m_aButton.toStdMap())
    {
        pair.second->setDown(pair.first == btnType);
    }
    m_pView->changeOperation(btnType);
    //todo: insert GIZMO for operation
}

CButtonConnector::CButtonConnector()
    :m_pView(nullptr)
{

}

CResourceManager *CResourceManager::getInstance()
{
    if(nullptr == m_resourceManager)
        m_resourceManager = new CResourceManager();
    return m_resourceManager;
}

CResourceManager::~CResourceManager()
{

}

void CResourceManager::init()
{
    QDir icoFolder(util::appPath() + QDir::separator() + "icons_fluent"); // todo: find ico in app folder and via options/configs

    if(icoFolder.exists())
        ei::log(eLogInfo, QString("%1:%2").arg("icon folder found:", icoFolder.path()) );
    else
    {
        ei::log(eLogWarning, QString("Icon folder not found: %1").arg(icoFolder.path()));
        return;
    }

    QStringList icoFilter;
    icoFilter << "*.ico" << "*.ICO";
    QFileInfoList icoFiles = icoFolder.entryInfoList(icoFilter);
    foreach(QFileInfo filename, icoFiles) {
        m_aIcon[filename.fileName()] = new QIcon(filename.absoluteFilePath());
    }
}

QIcon *CResourceManager::icon(QString name)
{
    if(m_aIcon.contains(name))
        return m_aIcon[name];
    ei::log(eLogWarning, "Icon " + name + " not found");
    return m_aIcon[QString("_emptyIco")];
}

CResourceManager::CResourceManager()
{
    m_aIcon[QString("_emptyIco")] = new QIcon(":/_emptyIco.ico");
}

QIcon *ei::icon(QString name)
{
    return CResourceManager::getInstance()->icon(name);
}
