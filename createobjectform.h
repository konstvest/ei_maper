#ifndef CREATEOBJECTFORM_H
#define CREATEOBJECTFORM_H

#include <QDialog>
#include <QSharedPointer>

#include "layout_components/tablemanager.h"
#include "node.h"

namespace Ui {
class CCreateObjectForm;
}

class CView;
class CPreview;

///
/// \brief The CCreateObjectForm class provides a form for creating a new object in the scene
///
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
    void onParamChange(const QSharedPointer<IPropertyBase>);

    void on_buttonCancel_clicked();
    void on_buttonCreate_clicked();

protected:
    void showEvent(QShowEvent* pEvent) override;

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
