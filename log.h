#ifndef LOG_H
#define LOG_H
#include <QTextEdit> // log window

class CLogger
{
public:
    CLogger() = delete;
    CLogger(QTextEdit* pTextEdit);
    ~CLogger();

    void log(const char* msg);
    void clear();
    void logToFile();

private:
    QTextEdit* m_pLog;
    QString m_fileName;
};

#endif // LOG_H
