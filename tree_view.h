#ifndef CTREEVIEW_H
#define CTREEVIEW_H
#include <QTreeWidget>
#include "types.h"

class CView;
class CTreeView : public QTreeWidget
{
    Q_OBJECT
public:
    explicit CTreeView(QWidget *parent = nullptr);
    void attachView(CView* pView) {m_pView = pView;}
    QString objectNameByType(ENodeType type);
    ENodeType objectTypeByString(QString str);

protected:
    void keyPressEvent(QKeyEvent* event) override;

private:
    int parentDeepCount(QTreeWidgetItem* pItem);

private slots:
    void itemClickedOver(QTreeWidgetItem* pItem, int column);

private:
    CView* m_pView;
};

#endif // CTREEVIEW_H
