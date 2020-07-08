#ifndef CLOGWINDOW_H
#define CLOGWINDOW_H

#include <QTextEdit>

class CLogWindow : public QTextEdit
{
    Q_OBJECT
public:
    explicit CLogWindow(QWidget *parent = nullptr);

signals:
    void test();

public slots:
    void log(QString msg);

};

#endif // CLOGWINDOW_H
