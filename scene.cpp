#include "scene.h"

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
}
