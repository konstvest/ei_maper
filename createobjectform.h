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
class CPreview;

class CCreateObjectForm : public QDialog
{
    Q_OBJECT

public:
    explicit CCreateObjectForm(QWidget *parent = nullptr);
    ~CCreateObjectForm();
    void attach(CView* pView, QUndoStack* pStack);
    CPreview* viewWidget();

    void closeEvent(QCloseEvent *event) override;

signals:
    void sendNewBbbox(CBox);

private slots:
    void onObjectChoose(QString& object);
    void onParamChange(SParam& param);

    void on_buttonCancel_clicked();
    void on_buttonCreate_clicked();

private:
    void initViewWidget();
    void updateTable();

private:
    Ui::CCreateObjectForm *ui;
    QMap<ENodeType, QString> m_objType;
    CNode* m_pNode;
    QSharedPointer<CTableManager> m_tableManager;
    CView* m_pView;
    CPreview * m_pPreview;
    QUndoStack* m_pUndoStack;
};

#endif // CREATEOBJECTFORM_H
