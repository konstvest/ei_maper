#include "scene.h"
#include "layout_components/ui_connectors.h"

CScene* CScene::m_pScene = nullptr;

CScene *CScene::getInstance()
{
    if(nullptr == m_pScene)
        m_pScene = new CScene();
    return m_pScene;
}

CScene::CScene():
    m_editMode(eEditModeObjects)
{

}

void CScene::changeMode(EEditMode mode)
{
    //TODO: add undo-redo for changing mode
    qDebug() << "switch edit mode to: " << mode;
    m_editMode = mode;
    //TODO: trigger ui tooltips in another place
    if(m_editMode == EEditMode::eEditModeLogic)
        CStatusConnector::getInstance()->updateStatus("select.ico", "LMB-Select object, Shift+LMB-Add to select, MMB-camera rotation, G-Move, P-Add Patrol(trap zone), L-Add look(cast point), CTLR+Tab-change mode");
    else
        CStatusConnector::getInstance()->updateStatus("select.ico", "LMB-Select object, Shift+LMB-Add to select, MMB-camera rotation, G-Move, T-Scale, R-Rotate, CTLR+Tab-change mode");
    emit modeChanged();
}
