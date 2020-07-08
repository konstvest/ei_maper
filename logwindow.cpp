#include <QDateTime>
#include "logwindow.h"

CLogWindow::CLogWindow(QWidget *parent) : QTextEdit(parent)
{
}

void CLogWindow::log(QString msg)
{
    QDateTime time = QDateTime::currentDateTime();
    QString text(time.toString(Qt::DateFormat::SystemLocaleShortDate));
    text.append(": " + msg + "\n");
    setReadOnly(false);
    insertPlainText(text);
    setReadOnly(true);
}
