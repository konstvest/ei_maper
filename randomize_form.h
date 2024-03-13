#ifndef RANDOMIZE_FORM_H
#define RANDOMIZE_FORM_H

#include <QWidget>
#include "types.h"

namespace Ui {
class CRandomizeForm;
}

class CView;

class CRandomizeForm : public QWidget
{
    Q_OBJECT

public:
    explicit CRandomizeForm(QWidget *parent = nullptr);
    ~CRandomizeForm();
    void attachView(CView* pView) {m_pView = pView;}

private slots:
    void on_push_Randomize_clicked();

    void on_push_Close_clicked();

private:
    void init();

private:
    Ui::CRandomizeForm *ui;
    QMap<QString, EObjParam> m_typeMap;
    CView* m_pView;
};

#endif // RANDOMIZE_FORM_H
