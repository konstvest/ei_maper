#ifndef CTREEVIEW_H
#define CTREEVIEW_H
#include <QTreeWidget>
#include "types.h"

class CView;
class CNode;

///
/// \brief The CTreeObject class stores additional information about the object ID so that it can be selected in the 3D scene.
///
class CTreeObject : public QTreeWidgetItem//, public QObject
{
    //Q_OBJECT
public:
    CTreeObject(QTreeWidget* pParent);
    CTreeObject(QTreeWidgetItem* pParent, uint nodeId);
    CTreeObject(CTreeObject* pParent, uint id);
    uint nodeId() {return m_nodeId;}
    void setId(uint id) {m_nodeId = id;}

private:
    uint m_nodeId;
};

///
/// \brief The CTreeView class provides display of objects on the scene
/// \todo display of patrol points (partially implemented)
///
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
    void recalcParent(QTreeWidgetItem* pItem);
    QTreeWidgetItem* category(QString& categoryName);

private slots:
    void itemClickedOver(QTreeWidgetItem* pItem, int column);
    void onNodeDelete(uint nodeId);
    void addNodeToTree(CNode* pNode);
    void onChangeNodeId(uint oldId, uint newId);
    void onChangeObjectName(CNode* pNode);

private:
    CView* m_pView;
};

#endif // CTREEVIEW_H
