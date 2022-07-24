#include "undo.h"
#include "node.h"
#include "main_window.h"
#include "mob.h"
#include "objects/unit.h"

COpenCommand::COpenCommand(CView* pView, QFileInfo& path, MainWindow* pMain, QUndoCommand *parent):
    QUndoCommand(parent)
    ,m_pView(pView)
    ,m_filePath(path)
    ,m_pMain(pMain)
{
    setText("load " + path.fileName());
}

void COpenCommand::undo()
{
    if(m_filePath.fileName().toLower().endsWith(".mpr"))
    {
        m_pView->unloadLand();
        m_pMain->setWindowTitle("ei_maper");
    }
    else
    {
        m_pView->unloadMob(m_filePath.fileName());
    }
}
void COpenCommand::redo()
{
    if(m_filePath.fileName().toLower().endsWith(".mpr"))
        m_pView->loadLandscape(m_filePath);
    else
        m_pView->loadMob(m_filePath);

}

//bool COpenCommand::mergeWith(const QUndoCommand* command)
//{
//    Q_UNUSED(command);
//    return true;
//}

CChangeStringParam::CChangeStringParam(CView* pView, uint nodeId, EObjParam objParam, QString value, QUndoCommand *parent):
    QUndoCommand(parent)
  ,m_pView(pView)
  ,m_nodeId(nodeId)
  ,m_objParam(objParam)
  ,m_newValue(value)
{
    //setText("Change value to:" + value);
}

void CChangeStringParam::undo()
{
    auto pNode = m_pView->currentMob()->nodeByMapId(m_nodeId);
    pNode->applyParam(m_objParam, m_oldValue);
    emit updateParam();
    //emit updatePosOnLand(m_pNode);
    m_pView->setDurty();
}

void CChangeStringParam::redo()
{
    setText("Change value to " + m_newValue);
    auto pNode = m_pView->currentMob()->nodeByMapId(m_nodeId);
    m_oldValue = pNode->getParam(m_objParam);
    pNode->applyParam(m_objParam, m_newValue);
    emit updateParam();
    //emit updatePosOnLand(m_pNode);
    m_pView->setDurty();
}

//bool CChangeObjectParam::mergeWith(const QUndoCommand *command)
//{
//    Q_UNUSED(command);
//    return true;
//}

CChangeModelParam::CChangeModelParam(CView* pView, uint nodeId, EObjParam &objParam, QString value, QUndoCommand *parent):
    CChangeStringParam(pView, nodeId, objParam, value, parent)
{
    //setText("Change model to " + value);
}

void CChangeModelParam::undo()
{
    auto pNode = m_pView->currentMob()->nodeByMapId(m_nodeId);
    pNode->applyParam(m_objParam, m_oldValue);
    pNode->applyParam(eObjParam_BODYPARTS, util::makeString(m_oldBodyparts));
    emit updatePosOnLand(pNode);
    emit updateParam();
    m_pView->setDurty();
}

void CChangeModelParam::redo()
{
    setText("Change model to " + m_newValue);
    auto pNode = m_pView->currentMob()->nodeByMapId(m_nodeId);
    m_oldValue = pNode->getParam(m_objParam);
    QString partsList = pNode->getParam(eObjParam_BODYPARTS);
    m_oldBodyparts = util::strListFromString(partsList);
    pNode->applyParam(m_objParam, m_newValue);
    QString empty("");
    pNode->applyParam(eObjParam_BODYPARTS, empty);
    emit updatePosOnLand(pNode);
    emit updateParam();
    m_pView->setDurty();
}

CDeleteNodeCommand::CDeleteNodeCommand(CView* pView, uint nodeId, QUndoCommand *parent)
    :QUndoCommand(parent)
    ,m_pView(pView)
    ,m_nodeId(nodeId)
{
}

void CDeleteNodeCommand::undo()
{
    m_pView->currentMob()->undo_deleteNode(m_nodeId);
    m_pView->setDurty();
}

void CDeleteNodeCommand::redo()
{
    auto pNode = m_pView->currentMob()->nodeByMapId(m_nodeId);
    setText(QString("Delete node ID: %1").arg(pNode->mapId()));
    m_pView->currentMob()->deleteNode(m_nodeId);
    m_pView->setDurty();
}

CCreateNodeCommand::CCreateNodeCommand(CView* pView, QJsonObject nodeData, QUndoCommand *parent):
    QUndoCommand(parent)
    ,m_pView(pView)
    ,m_nodeData(nodeData)
    ,m_createdNodeId(0)
{
    //setText("Node created");
}

void CCreateNodeCommand::undo()
{
    m_pView->currentMob()->undo_createNode(m_createdNodeId);
    m_pView->setDurty();
}

void CCreateNodeCommand::redo()
{
    auto pNode = m_pView->currentMob()->createNode(m_nodeData);



    m_createdNodeId = pNode->mapId();
    setText("Node created ID: " + QString::number(pNode->mapId()));
    m_pView->setDurty();
}

CChangeLogicParam::CChangeLogicParam(CView* pView, CNode *pPoint, EObjParam objParam, QString value, QUndoCommand *parent):
    QUndoCommand(parent)
  ,m_pView(pView)
  ,m_pPoint(pPoint)
  ,m_objParam(objParam)
  ,m_newValue(value)
{

}

void CChangeLogicParam::undo()
{
    m_pPoint->applyLogicParam(m_objParam, m_oldValue);
    emit updateParam();
    //emit updatePosOnLand(m_pNode);
    m_pView->setDurty();
}

void CChangeLogicParam::redo()
{
    setText("Change value to " + m_newValue);
    m_oldValue = m_pPoint->getLogicParam(m_objParam);
    m_pPoint->applyLogicParam(m_objParam, m_newValue);
    emit updateParam();
    //emit updatePosOnLand(m_pNode);
    m_pView->setDurty();
}

CDeletePatrol::CDeletePatrol(CNode *pNode, QUndoCommand *parent):
    QUndoCommand(parent)
  ,m_pNode(pNode)
{

}

void CDeletePatrol::undo()
{
    m_pNode->markAsDeleted(false);
}

void CDeletePatrol::redo()
{
    setText("Delete logic node");
    m_pNode->markAsDeleted(true);
}

CCreatePatrolCommand::CCreatePatrolCommand(CView* pView, CPatrolPoint *pBasePoint, QUndoCommand *parent):
    QUndoCommand(parent)
  ,m_pView(pView)
  ,m_pBasePoint(pBasePoint)
{
}

void CCreatePatrolCommand::undo()
{
    m_pBasePoint->undo_createNewPoint(m_pCreatedPoint);
    m_pView->currentMob()->logicNodesUpdate();
}

void CCreatePatrolCommand::redo()
{
    m_pCreatedPoint = m_pBasePoint->createNewPoint();
    m_pView->currentMob()->logicNodesUpdate();
    m_pCreatedPoint->setState(ENodeState::eSelect);
    setText("Created new Patrol point");
}

CCreateUnitPatrolCommand::CCreateUnitPatrolCommand(CView *pView, CUnit *pUnit, QUndoCommand *parent):
    QUndoCommand(parent)
  ,m_pView(pView)
  ,m_pUnit(pUnit)
{
}

void CCreateUnitPatrolCommand::undo()
{
    m_pUnit->undo_addFirstPatrolPoint();
    m_pView->currentMob()->logicNodesUpdate();
}

void CCreateUnitPatrolCommand::redo()
{
    m_pUnit->addFirstPatrolPoint();
    setText("Added first patrol");
    m_pView->currentMob()->logicNodesUpdate();
}

CCreateViewCommand::CCreateViewCommand(CView *pView, CLookPoint *pBasePoint, QUndoCommand *parent):
    QUndoCommand(parent)
  ,m_pView(pView)
  ,m_pBasePoint(pBasePoint)
{
}

void CCreateViewCommand::undo()
{
    m_pBasePoint->undo_createLookPoint(m_pCreatedPoint);
    m_pView->currentMob()->logicNodesUpdate();
}

void CCreateViewCommand::redo()
{
    m_pCreatedPoint = m_pBasePoint->createLookPoint();
    m_pView->currentMob()->logicNodesUpdate();
    m_pCreatedPoint->setState(ENodeState::eSelect);
    setText("Created new Look point");
}

CCreatePatrolViewCommand::CCreatePatrolViewCommand(CView *pView, CPatrolPoint *pPoint, QUndoCommand *parent):
    QUndoCommand(parent)
  ,m_pView(pView)
  ,m_pPoint(pPoint)
{
}

void CCreatePatrolViewCommand::undo()
{
    m_pPoint->undo_addFirstViewPoint();
    m_pView->currentMob()->logicNodesUpdate();
}

void CCreatePatrolViewCommand::redo()
{
    m_pPoint->addFirstViewPoint();
    m_pView->currentMob()->logicNodesUpdate();
    setText("Created view point");
}
