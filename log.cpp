#include "log.h"
#include <QFile>
#include <QTextStream>

CLogger::CLogger(QTextEdit* pTextEdit):
    m_pLog(pTextEdit)
  ,m_fileName("~workflow.log")
{
    if(QFile::exists(m_fileName))
        QFile::remove(m_fileName);
}

CLogger::~CLogger()
{
    logToFile();
}

void CLogger::log(const char* msg)
{
    QString message(msg);
    message.append('\n');
    m_pLog->setReadOnly(false);
    m_pLog->insertPlainText(message);
    m_pLog->setReadOnly(true);
    //todo: update log window in realtime
    //m_pLog->update();
}

void CLogger::clear()
{
    m_pLog->clear();
}

void CLogger::logToFile()
{
    QFile outfile;
    outfile.setFileName(m_fileName);
    outfile.open(QIODevice::Append | QIODevice::Text);
    QTextStream out(&outfile);
    out << m_pLog->toPlainText() << endl;
}
