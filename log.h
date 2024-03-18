#ifndef LOG_H
#define LOG_H

#include <QString>
#include <QFile>

#define LOG_FATAL(msg) ei::log(eLogFatal, msg, Q_FUNC_INFO)

enum ELogMessageType
{
    eLogInfo = 0
    ,eLogWarning
    ,eLogError
    ,eLogFatal
    ,eLogDebug
    ,eLogStart
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

private:
    CLogger();
    ~CLogger();

private:
    static CLogger* m_pLogger;
    QFile log_file;
    CSettings* m_pSettings;
};

namespace ei {
void log(ELogMessageType type, const char* msg);
void log(ELogMessageType type, const QString msg);
void log(ELogMessageType type, const QString msg, QString func);
}

#endif // LOG_H
