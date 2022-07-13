#ifndef CREATEOBJECTFORM_H
#define CREATEOBJECTFORM_H

#include <QDialog>
#include <QSharedPointer>

#include "tablemanager.h"
#include "node.h"

namespace Ui {
class CCreateObjectForm;
}

class CView;

class CCreateObjectForm : public QDialog
{
    Q_OBJECT

public:
    explicit CCreateObjectForm(QWidget *parent = nullptr);
    ~CCreateObjectForm();
    void attachView(CView* pView) {m_pView = pView;}
    void updateTable();

private slots:
    void onObjectChoose(const QString& object);
    void onParamChange(SParam& param);

    void on_buttonCancel_clicked();

    void on_buttonCreate_clicked();

private:
    Ui::CCreateObjectForm *ui;
    QMap<ENodeType, QString> m_objType;
    CNode* m_pNode;
    QSharedPointer<CTableManager> m_tableManager;
    CView* m_pView;
};

#endif // CREATEOBJECTFORM_H
