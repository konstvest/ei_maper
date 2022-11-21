#include "tree_view.h"
#include <QKeyEvent>
#include <QDebug>
#include "view.h"

CTreeView::CTreeView(QWidget *parent) : QTreeWidget(parent)
{
    QObject::connect(this, SIGNAL(itemClicked(QTreeWidgetItem*,int)), this, SLOT(itemClickedOver(QTreeWidgetItem*,int)));
}

void CTreeView::keyPressEvent(QKeyEvent *pEvent)
{
    auto key = pEvent->key();
    if(key == Qt::Key_Delete)
    {
        auto pItem = currentItem();
        auto column = currentColumn();
        if(column != 0 || nullptr == pItem) // clicked on count field. ignore it
            return;

        int parentCount = parentDeepCount(pItem);
        if(parentCount > 0)
        {
            m_pView->deleteSelectedNodes();
            pItem->parent()->removeChild(pItem);
        }
    }

}

int CTreeView::parentDeepCount(QTreeWidgetItem *pItem)
{
    int parentCount=0;
    auto pParent = pItem->parent();
    while(nullptr != pParent)
    {
        pParent = pParent->parent();
        ++parentCount;
    }
    return parentCount;
}

QString CTreeView::objectNameByType(ENodeType type)
{
    QMap<ENodeType, QString> type2str;
    type2str.insert(ENodeType::eWorldObject, "World objects");
    type2str.insert(ENodeType::eUnit, "Units");
    type2str.insert(ENodeType::eTorch, "Torches");
    type2str.insert(ENodeType::eLever, "Levers");
    type2str.insert(ENodeType::eMagicTrap, "Magic traps");
    type2str.insert(ENodeType::eLight, "Light source");
    type2str.insert(ENodeType::eSound, "Sound sources");
    type2str.insert(ENodeType::eParticle, "Particle sources");

    return type2str[type];
}

ENodeType CTreeView::objectTypeByString(QString str)
{
    QMap<ENodeType, QString> type2str;
    type2str.insert(ENodeType::eWorldObject, "World objects");
    type2str.insert(ENodeType::eUnit, "Units");
    type2str.insert(ENodeType::eTorch, "Torches");
    type2str.insert(ENodeType::eLever, "Levers");
    type2str.insert(ENodeType::eMagicTrap, "Magic traps");
    type2str.insert(ENodeType::eLight, "Light source");
    type2str.insert(ENodeType::eSound, "Sound sources");
    type2str.insert(ENodeType::eParticle, "Particle sources");
    return type2str.key(str);
}

void CTreeView::itemClickedOver(QTreeWidgetItem *pItem, int column)
{
    if(column != 0) // clicked on count field. ignore it
        return;

    int parentCount = parentDeepCount(pItem);

    SSelect sel;
    switch (parentCount)
    {
    case 0:
        return; // for now skip clicking root items
    case 1:
    { // first child
        ENodeType baseType = objectTypeByString(pItem->parent()->text(0));
        sel.type = baseType == ENodeType::eUnit ? eSelectType_Database_name : eSelectType_Map_name;
        sel.param1 = pItem->text(0);
        sel.param2 = sel.param1; //for hard comparison
        m_pView->select(sel, false);
        if(pItem->text(1).toInt() == 1)
            m_pView->moveCamToSelectedObject();
        break;
    }
    case 2:
    { // second child
        //ENodeType baseType = objectTypeByString(pItem->parent()->parent()->text(0));
        sel.type = eSelectType_Id_range;
        sel.param1 = pItem->text(0);
        sel.param2 = pItem->text(0);
        m_pView->select(sel, false);
        m_pView->moveCamToSelectedObject();
        break;
    }

    }
}
