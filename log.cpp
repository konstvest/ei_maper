#include "log.h"
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QCoreApplication>
#include <QDebug>
#include "settings.h"

CLogger* CLogger::m_pLogger = nullptr;

CLogger *CLogger::getInstance()
{
    if(nullptr == m_pLogger)
        m_pLogger = new CLogger();
    return m_pLogger;
}

void CLogger::log(ELogMessageType type, const char *msg)
{
    log(type, QString(msg));
}

void CLogger::log(ELogMessageType type, const QString msg)
{
    if(m_loglvl<type)
        return;

    QString dt = QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss");
    QString txt = QString("[%1] ").arg(dt);

    QTextStream textStream(&log_file);
    switch (type)
    {
    case eLogInfo:
        txt += QString("{INFO} \t %1").arg(msg); break;
    case eLogDebug:
        txt += QString("{DEBUG} \t %1").arg(msg); break;
    case eLogWarning:
        txt += QString("{WARNING} \t %1").arg(msg); break;
    case eLogFatal:
        txt += QString("{ERROR} \t %1").arg(msg); break;
    case eLogInit:
        txt = QString("[   %1\t  %2  ] {%3} \t %4").arg("DATE", "TIME", "TYPE", "MESSAGE"); break;
    }

    log_file.open(QIODevice::WriteOnly | QIODevice::Append);
    textStream << txt << endl;
    log_file.close();

#ifdef QT_DEBUG
  qDebug() << txt;
#endif

//    if(type == eLogFatal)
    //        QCoreApplication::exit(-1);
}

void CLogger::attachSettings(CSettings *pSet)
{
    m_pSettings = pSet;
}

CLogger::CLogger()
    :log_file("workflow.log")
    ,m_pSettings(nullptr)
    ,m_loglvl(eLogDebug)
{
    log(eLogInit, "Log started");
}

void CLogger::setLogLevel(ELogMessageType type)
{
    m_loglvl = type;
}

CLogger::~CLogger()
{//this method never used bcs singleton is a singleton!!!
    if(nullptr != m_pLogger)
    {
        delete m_pLogger;
    }
}

void ei::log(ELogMessageType type, const QString msg)
{
    CLogger::getInstance()->log(type, msg);
}

void ei::log(ELogMessageType type, const char *msg)
{
    CLogger::getInstance()->log(type, msg);
}

void ei::log(ELogMessageType type, const QString msg, QString func)
{
    CLogger::getInstance()->log(type, msg +"\t|" + func);
}
