#ifndef LOG_H
#define LOG_H

#include <QString>
#include <QFile>

#define LOG_FATAL(msg) ei::log(eLogFatal, msg, Q_FUNC_INFO)

enum ELogMessageType
{
    eLogInit = 0
    ,eLogFatal
    ,eLogWarning
    ,eLogInfo
    ,eLogDebug
};

class CSettings;

///
/// \brief The CLogger class provides logging
///
class CLogger
{
public:
    static CLogger* getInstance();
    void log(ELogMessageType type, const char* msg);
    void log(ELogMessageType type, const QString msg);
    void attachSettings(CSettings* pSet);

    CLogger(CLogger const&) = delete;
    void operator=(CLogger const&)  = delete;
    void setLogLevel(ELogMessageType type);

private:
    CLogger();
    ~CLogger();

private:
    static CLogger* m_pLogger;
    QFile log_file;
    CSettings* m_pSettings;
    ELogMessageType m_loglvl;
};

namespace ei {
void log(ELogMessageType type, const char* msg);
void log(ELogMessageType type, const QString msg);
void log(ELogMessageType type, const QString msg, QString func);
}

#endif // LOG_H
