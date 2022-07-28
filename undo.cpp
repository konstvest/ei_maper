#include "undo.h"
#include "node.h"
#include "main_window.h"
#include "mob.h"
#include "objects/unit.h"
#include "objects/magictrap.h"

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

CChangeLogicParam::CChangeLogicParam(CView* pView, QString pointHash, EObjParam objParam, QString value, QUndoCommand *parent):
    QUndoCommand(parent)
  ,m_pView(pView)
  ,m_pointHash(pointHash)
  ,m_objParam(objParam)
  ,m_newValue(value)
{
}

void CChangeLogicParam::undo()
{
    QStringList list = m_pointHash.split(".");
    if(list.size()==1)
    {//unit
        auto pUnit = m_pView->currentMob()->nodeByMapId(list[0].toUInt());
        pUnit->applyLogicParam(m_objParam, m_oldValue);
    }
    else if(list.size()==2)
    {//patrol point
        CPatrolPoint* pPoint = m_pView->currentMob()->patrolPointById(list[0].toInt(), list[1].toInt());
        pPoint->applyLogicParam(m_objParam, m_oldValue);
    }
    else if(list.size()==3)
    {//look point
        CLookPoint* pPoint = m_pView->currentMob()->viewPointById(list[0].toInt(), list[1].toInt(), list[2].toInt());
        pPoint->applyLogicParam(m_objParam, m_oldValue);
    }
    else if(list.size() == 4) //trap zone
    {
        CActivationZone* pZone = m_pView->currentMob()->actZoneById(list[0].toInt(), list[3].toInt());
        pZone->applyLogicParam(m_objParam, m_oldValue);
    }
    else if(list.size() == 5) //trap cast point
    {
        CTrapCastPoint* pCast = m_pView->currentMob()->trapCastById(list[0].toInt(), list[4].toInt());
        pCast->applyLogicParam(m_objParam, m_oldValue);
    }

    emit updateParam();
    m_pView->setDurty();
}

void CChangeLogicParam::redo()
{
    QStringList list = m_pointHash.split(".");
    if(list.size()==1)
    {//unit
        auto pUnit = m_pView->currentMob()->nodeByMapId(list[0].toUInt()); //can be magic trap
        m_oldValue = pUnit->getLogicParam(m_objParam);
        pUnit->applyLogicParam(m_objParam, m_newValue);
    }
    else if(list.size()==2)
    {//patrol point
        CPatrolPoint* pPoint = m_pView->currentMob()->patrolPointById(list[0].toInt(), list[1].toInt());
        m_oldValue = pPoint->getLogicParam(m_objParam);
        pPoint->applyLogicParam(m_objParam, m_newValue);
    }
    else if(list.size()==3)
    {//look point
        CLookPoint* pPoint = m_pView->currentMob()->viewPointById(list[0].toInt(), list[1].toInt(), list[2].toInt());
        m_oldValue = pPoint->getLogicParam(m_objParam);
        pPoint->applyLogicParam(m_objParam, m_newValue);
    }
    else if(list.size() == 4) //trap zone
    {
        CActivationZone* pZone = m_pView->currentMob()->actZoneById(list[0].toInt(), list[3].toInt());
        m_oldValue = pZone->getLogicParam(m_objParam);
        pZone->applyLogicParam(m_objParam, m_newValue);
    }
    else if(list.size() == 5) //trap cast point
    {
        CTrapCastPoint* pCast = m_pView->currentMob()->trapCastById(list[0].toInt(), list[4].toInt());
        m_oldValue = pCast->getLogicParam(m_objParam);
        pCast->applyLogicParam(m_objParam, m_newValue);
    }

    emit updateParam();
    m_pView->setDurty();
    setText("Change value to " + m_newValue);
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

CCreatePatrolCommand::CCreatePatrolCommand(CView* pView, QString pointHash, QUndoCommand *parent):
    QUndoCommand(parent)
  ,m_pView(pView)
  ,m_pointHash(pointHash)
{
}

void CCreatePatrolCommand::undo()
{
    m_pView->currentMob()->undo_createPatrolByHash(m_pointHash);
}

void CCreatePatrolCommand::redo()
{
    m_pView->currentMob()->createPatrolByHash(m_pointHash);
    setText("Created new point");
}
