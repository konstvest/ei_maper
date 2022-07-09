#ifndef CREATEOBJECTFORM_H
#define CREATEOBJECTFORM_H

#include <QDialog>
#include <QSharedPointer>

#include "tablemanager.h"
#include "node.h"

namespace Ui {
class CCreateObjectForm;
}

class CCreateObjectForm : public QDialog
{
    Q_OBJECT

public:
    explicit CCreateObjectForm(QWidget *parent = nullptr);
    ~CCreateObjectForm();

    void updateTable();

private slots:
    void onObjectChoose(const QString& object);
    void onParamChange(SParam& param);

private:
    Ui::CCreateObjectForm *ui;
    QMap<ENodeType, QString> m_objType;
    CNode* m_pNode;
    QSharedPointer<CTableManager> m_tableManager;
};

#endif // CREATEOBJECTFORM_H
