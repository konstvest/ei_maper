#include "undo.h"
#include "node.h"
#include "mainwindow.h"
#include "mob.h"

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
    {
        m_pView->loadLandscape(m_filePath);
        m_pMain->setWindowTitle("ei_maper (" + m_filePath.baseName() + ")");
    }
    else
    {
        m_pView->loadMob(m_filePath);
    }
}

//bool COpenCommand::mergeWith(const QUndoCommand* command)
//{
//    Q_UNUSED(command);
//    return true;
//}

CChangeStringParam::CChangeStringParam(CNode *pNode, EObjParam objParam, QString value, QUndoCommand *parent):
    QUndoCommand(parent)
  ,m_pNode(pNode)
  ,m_objParam(objParam)
  ,m_newValue(value)
{
    setText("Change value to:" + value);
}

void CChangeStringParam::undo()
{
    m_pNode->applyParam(m_objParam, m_oldValue);
    emit updateParam();
    //emit updatePosOnLand(m_pNode);
}

void CChangeStringParam::redo()
{
    m_oldValue = m_pNode->getParam(m_objParam);
    m_pNode->applyParam(m_objParam, m_newValue);
    emit updateParam();
    //emit updatePosOnLand(m_pNode);
}

//bool CChangeObjectParam::mergeWith(const QUndoCommand *command)
//{
//    Q_UNUSED(command);
//    return true;
//}

CChangeModelParam::CChangeModelParam(CNode *pNode, EObjParam &objParam, QString value, QUndoCommand *parent):
    CChangeStringParam(pNode, objParam, value, parent)
{
    setText("Change model to " + value);
}

void CChangeModelParam::undo()
{
    m_pNode->applyParam(m_objParam, m_oldValue);
    m_pNode->applyParam(eObjParam_BODYPARTS, util::makeString(m_oldBodyparts));
    emit updatePosOnLand(m_pNode);
    emit updateParam();
}

void CChangeModelParam::redo()
{
    m_oldValue = m_pNode->getParam(m_objParam);
    QString partsList = m_pNode->getParam(eObjParam_BODYPARTS);
    m_oldBodyparts = util::strListFromString(partsList);
    m_pNode->applyParam(m_objParam, m_newValue);
    QString empty("");
    m_pNode->applyParam(eObjParam_BODYPARTS, empty);
    emit updatePosOnLand(m_pNode);
    emit updateParam();
}

CDeleteNodeCommand::CDeleteNodeCommand(CMob *pMob, CNode *pNode, QUndoCommand *parent)
    :QUndoCommand(parent)
    ,m_pMob(pMob)
    ,m_pNode(pNode)
{
    setText("Node deleted ID: " + QString::number(m_pNode->mapId()));
}

void CDeleteNodeCommand::undo()
{
    m_pMob->restoreNode(m_pNode->innerId());
}

void CDeleteNodeCommand::redo()
{
    m_pMob->deleteNode(m_pNode);
}

CCreateNodeCommand::CCreateNodeCommand(CMob* pMob, QJsonObject nodeData, QUndoCommand *parent):
    QUndoCommand(parent)
    ,m_pMob(pMob)
    ,m_nodeData(nodeData)
    ,m_pNode(nullptr)
{
    setText("Node created");
}

void CCreateNodeCommand::undo()
{
    m_pMob->deleteNode(m_pNode);

}

void CCreateNodeCommand::redo()
{
    m_pNode = m_pMob->createNode(m_nodeData);
    setText("Node created ID: " + QString::number(m_pNode->mapId()));
}
