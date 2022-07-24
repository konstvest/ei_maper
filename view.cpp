#include <QtWidgets>
#include <QDebug>
#include <QtMath>       //for fabs
#include <QMap>         //for parameters
#include <QAbstractScrollArea>
#include <QUndoStack>
#include <QTreeWidget>

#include "view.h"
#include "camera.h"
#include "mob.h"
#include "resourcemanager.h"
#include "node.h"
#include "landscape.h"
#include "key_manager.h"
#include "log.h"
#include "settings.h"
#include "undo.h"
#include "objects/object_base.h"
#include "objects/worldobj.h"
#include "objects/unit.h"
#include "mobparameters.h"
#include "progressview.h"
#include "tablemanager.h"
#include "operationmanager.h"
#include "ogl_utils.h"
#include "scene.h"

class CLogic;

CView::CView(QWidget *parent, const QGLWidget *pShareWidget):
    QGLWidget(parent, pShareWidget)
    , m_pSettings(nullptr)
    , m_pProgress(nullptr)
    , m_clipboard_buffer_file(QString("%1%2%3").arg(QDir::tempPath()).arg(QDir::separator()).arg("copy_paste_buffer.json"))
    , m_activeMob(nullptr)
    ,m_pTree(nullptr)
{
    setFocusPolicy(Qt::ClickFocus);

    m_cam.reset(new CCamera);
    m_timer = new QTimer;
    m_aReadState.resize(eReadCount);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(updateWindow()));
    m_operationBackup.clear();
    m_selectFrame.reset(new CSelectFrame);
    //qDebug() << format();
    //qDebug() << isSharing();
}



void CView::updateReadState(EReadState state)
{
    m_aReadState[state] = true;
    qDebug() << state << " read";
}

CView::~CView()
{
    for(auto& mob: m_aMob)
        delete mob;
}


void CView::attach(CSettings* pSettings, QTableWidget* pParam, QUndoStack* pStack, CProgressView* pProgress, QLineEdit* pMouseCoord, QTreeWidget* pTree) //todo: use signal\slots
{
    m_pSettings = pSettings;
    m_cam->attachSettings(pSettings);
    m_cam->reset();
    m_tableManager.reset(new CTableManager(pParam));
    QObject::connect(m_tableManager.get(), SIGNAL(changeParamSignal(SParam&)), this, SLOT(onParamChange(SParam&)));
    m_pUndoStack = pStack;

    m_pProgress = pProgress;
    m_pOp.reset(new COperation(new CSelect(this)));
    m_pOp->attachCam(m_cam.get());
    m_cam->attachKeyManager(m_pOp->keyManager());
    m_pOp->attachkMouseCoordFiled(pMouseCoord);
    CLogger::getInstance()->attachSettings(pSettings);

    m_pTree = pTree;
    pTree->setColumnCount(1);
    pTree->setHeaderLabel("Object list");
    auto pTodoItem = new QTreeWidgetItem(pTree);
    pTree->addTopLevelItem(pTodoItem);
    pTodoItem->setText(0, "Здесь могла быть ваша реклама");
}

void CView::updateWindow()
{
    updateGL();
}

void CView::initShaders()
{
    // Compile vertex shader
    if (!m_program.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/vshader.vsh"))
        close();

    // Compile fragment shader
    if (!m_program.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/fshader.fsh"))
        close();

    // Link shader pipeline
    if (!m_program.link())
        close();

    m_program.bind();
    m_program.setUniformValue("customColor", QVector4D(0.0, 0.0, 0.0, 0.0));
    //release shader program?

    // Compile land shader
    if (!m_landProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/vshader.vsh"))
        close();
    if (!m_landProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/landshader.fsh"))  //use absolute path for recompile shader every app rise
        close();
    if (!m_landProgram.link())
        close();

    m_landProgram.bind();
    m_landProgram.setUniformValue("u_lightPosition", QVector4D(0.0, 0.0, 2.0, 1.0));
    m_landProgram.setUniformValue("u_lightPower", 5.0f);
    m_landProgram.setUniformValue("u_lightColor", QVector4D(1.0, 1.0, 1.0, 1.0));
    m_landProgram.setUniformValue("u_highlight", false);


    // Compile select shader
    if (!m_selectProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/vshader.vsh"))
        close();
    if (!m_selectProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/select.fsh"))
        close();
    if (!m_selectProgram.link())
        close();

}

void CView::initializeGL()
{
    makeCurrent();
    qglClearColor(Qt::black);
    //glEnable(GL_EXT_texture_filter_anisotropic);
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE); //draw only front faces (disable if need draw both sides)

    glEnable(GL_ALPHA_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    initShaders();
    CTextureList::getInstance()->initResource();
    CObjectList::getInstance()->initResource();
    //loadResource();
    //doneCurrent();
}

void CView::resizeGL(int width, int height)
{
    makeCurrent();
    m_height = height;
    m_width = width;
    //const int side = qMin(width, height);
    //glViewport((width - side) / 2, (height - side) / 2, side, side);
    const float ratio = float(width)/height;
    glViewport(0, 0, width, height);

    m_projection.setToIdentity();
    m_projection.perspective(45, ratio, 1, 1000);
    //glOrtho(-2, +2, -2, +2, -2.0, 2.0);   // draw objects in orthogaphic view
    //glFrustum (-2.0, 2.0, -2.0, 2.0, 1.5, 20.0); //convert project matrix to perpective view
    //doneCurrent();
}

void CView::paintGL()
{
    makeCurrent();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    draw();
    //doneCurrent();
}

void CView::draw()
{

    QMatrix4x4 camMatrix = m_cam->update();
    // Set modelview-projection matrix

    // Bind shader pipeline for use
    if (!m_landProgram.bind())
        close();

    // Set modelview-projection matrix
    //m_landProgram.setUniformValue("u_watcherPos", m_cam->pos());
    m_landProgram.setUniformValue("u_projMmatrix", m_projection);
    m_landProgram.setUniformValue("u_viewMmatrix", camMatrix);
//    m_landProgram.setUniformValue("u_color", QVector4D(0.0, 1.0, 0.0, 1.0));
//    m_landProgram.setUniformValue("u_lightPosition", QVector4D(0.0, 0.0, 2.0, 1.0));
//    m_landProgram.setUniformValue("u_lightPower", 5.0f);
//    m_landProgram.setUniformValue("u_lightColor", QVector4D(1.0, 1.0, 1.0, 1.0));
//    m_landProgram.setUniformValue("u_highlight", false);
    auto pLand = CLandscape::getInstance();
    if (pLand && pLand->isMprLoad())
        pLand->draw(&m_landProgram);

    // Bind shader pipeline for use
    if (!m_program.bind())
        close();

    m_program.setUniformValue("u_projMmatrix", m_projection);
    m_program.setUniformValue("u_viewMmatrix", camMatrix);

    for(auto& mob: m_aMob) //TODO: draw inactive mobs with custom color|alfa or something
        for (auto& node: mob->nodes())
            node->draw(&m_program);

    if (pLand && pLand->isMprLoad())
    {
        COptBool* pOpt = dynamic_cast<COptBool*>(settings()->opt("drawWater"));
        if (pOpt and pOpt->value() == true)
        {
            //turn to landshader again
            if (!m_landProgram.bind())
                close();

            m_landProgram.setUniformValue("transparency", 0.3f);
            pLand->drawWater(&m_landProgram);
            m_landProgram.setUniformValue("transparency", 0.0f);
        }
    }

    //draw selection frame using m_program shader
    if (!m_program.bind())
        close();

    if (pLand && pLand->isMprLoad() && !m_aMob.isEmpty())
    {
        QMatrix4x4 mtrx;
        mtrx.setToIdentity();
        m_program.setUniformValue("u_projMmatrix", mtrx);
        m_program.setUniformValue("u_viewMmatrix", mtrx);
        m_selectFrame->draw(&m_program);
    }
}

void CView::loadLandscape(QFileInfo& filePath)
{
    if(CLandscape::getInstance()->isMprLoad())
    {
        QMessageBox::warning(this, "Warning","Landscape already loaded. Please close before opening new zone (*mpr)");
        //LOG_FATAL("ahtung"); //test logging critial error
        return;
    }

    ei::log(eLogInfo, "Start read landscape");
    CLandscape::getInstance()->readMap(filePath);
    emit updateMainWindowTitle(eTitleTypeData::eTitleTypeDataMpr, filePath.baseName());
    ei::log(eLogInfo, "End read landscape");
    m_timer->setInterval(15); //"fps" for drawing
    m_timer->start();
}

void CView::unloadLand()
{
    CLandscape::getInstance()->unloadMpr();
    emit updateMainWindowTitle(eTitleTypeData::eTitleTypeDataMpr, "");
    ei::log(eLogInfo, "Landscape unloaded");
}

int CView::select(const SSelect &selectParam, bool bAddToSelect)
{
    for (const auto& mob : m_aMob)
    {
        const QString mobName = mob->mobName().toLower();
        for (auto& node : mob->nodes())
        {
            switch (selectParam.type) {
            case eSelectType_Id_range:
            {
                uint id_min = selectParam.param1.toUInt();
                uint id_max = selectParam.param2.toUInt();
                if (id_max < id_min)
                {
                    uint temp = id_min;
                    id_min = id_max;
                    id_max = temp;
                }
                if (node->mapId() >= id_min && node->mapId() <=id_max)
                {
                    node->setState(eSelect);
                }
                else if (!bAddToSelect && (node->nodeState() & eSelect)) //deselect
                    node->setState(eDraw);
                break;
            }
            case eSelectType_Map_name:
            {
                auto pObj = dynamic_cast<CObjectBase*>(node);
                if (!pObj)
                    break;
                if (!selectParam.param1.isEmpty()
                        && (node->prototypeName().toLower().contains(selectParam.param1.toLower())))
                {
                    node->setState(eSelect);
                }
                else if (!bAddToSelect && (node->nodeState() & eSelect)) //deselect
                    node->setState(eDraw);
                break;
            }
            case eSelectType_Texture_name:
            {
                if (!(node->nodeType() & ENodeType::eWorldObject))
                    break;
                if (!selectParam.param1.isEmpty()
                        && (node->textureName().toLower().contains(selectParam.param1.toLower())))
                {
                    node->setState(eSelect);
                }
                else if (!bAddToSelect && (node->nodeState() & eSelect)) //deselect
                    node->setState(eDraw);
                break;
            }
            case eSelectType_Model_name:
            {
                if (!(node->nodeType() & ENodeType::eWorldObject))
                    break;
                if (!selectParam.param1.isEmpty()
                        && (node->modelName().toLower().contains(selectParam.param1.toLower())))
                {
                    node->setState(eSelect);
                }
                else if (!bAddToSelect && (node->nodeState() & eSelect)) //deselect
                    node->setState(eDraw);
                break;
            }
            case eSelectType_Mob_file:
            {
                if(mobName.toLower().contains(selectParam.param1.toLower()))
                {
                    node->setState(eSelect);
                }
                else if (!bAddToSelect && (node->nodeState() & eSelect)) //deselect
                    node->setState(eDraw);
                break;
            }
            case eSelectType_Position_circle:
            {
                //TODO
                break;
            }
            case eSelectType_Position_rectangle:
            {
                //TODO
                break;
            }
            case eSelectType_Diplomacy_group:
            {
                auto pObj = dynamic_cast<CWorldObj*>(node);
                if (!pObj)
                    break;

                int group_min = selectParam.param1.toUInt();
                int group_max = selectParam.param2.toUInt();
                if (group_max < group_min)
                {
                    uint temp = group_min;
                    group_min = group_max;
                    group_max = temp;
                }
                if (pObj->dipGroup() >= group_min && pObj->dipGroup() <= group_max )
                {
                    node->setState(eSelect);
                }
                else if (!bAddToSelect && (node->nodeState() & eSelect)) //deselect
                {
                    node->setState(eDraw);
                }
                break;
            }
            case eSelectType_Database_name:
            {
                auto pObj = dynamic_cast<CUnit*>(node);
                if (!pObj)
                    break;

                if (pObj->databaseName().toLower().contains(selectParam.param1.toLower()))
                {
                    node->setState(eSelect);
                }
                else if (!bAddToSelect && (node->nodeState() & eSelect)) //deselect
                {
                    node->setState(eDraw);
                }
                break;
            }
            case eSelectType_all:
            {
                node->setState(eSelect);
                break;
            }
            default:
                break;
            }
        }
    }
    viewParameters();
    return cauntSelectedNodes();
}

CMob *CView::mob(QString mobName)
{
    CMob* pMob = nullptr;
    foreach(pMob, m_aMob)
        if (pMob->mobName().toLower() == mobName.toLower())
            break;

    return pMob;
}

void CView::pickObject(QPoint mousePos, bool bAddToSelect)
{
    QRect rect(mousePos, mousePos);
    pickObject(rect, bAddToSelect);
    return;
}

QDebug operator<<(QDebug stream, const SColor& color)
{
    stream << '{' << color.rgb[0] << ',' << color.rgb[1] << ',' << color.rgb[2];
    if (color.hasAlpha)
        stream << ',' << color.rgb[3];
    stream << '}';
    return stream;
}

void CView::getColorFromRect(const QRect& rect, QVector<SColor>& aColor)
{
    //get colors from rectangle
    const int component = 3; //rgb
    //revert Oy for picking
    const QPoint topLeft (rect.left(), m_height - rect.bottom());
    const QPoint btmRight(rect.right(), m_height - rect.top());
    const QRect inversedRect(topLeft, btmRight);

    const int h = inversedRect.height() == 0 ? 1 : qAbs(inversedRect.height());
    const int w = inversedRect.width() == 0 ? 1 :qAbs(inversedRect.width());
    uint size = component * w * h;
    QByteArray* pixel = new QByteArray();
    pixel->resize(size);
    glPixelStorei(GL_PACK_ALIGNMENT, 1); // glReadPixels use 4 component by default. for RGB must define pack alignment
    glReadPixels(inversedRect.left(), inversedRect.bottom(), w, h, GL_RGB, GL_UNSIGNED_BYTE, pixel->data());
    SColor color;
    for(int i(0); i<component * w * h; i+=3)
    {
        color = SColor(pixel->at(i), pixel->at(i+1), pixel->at(i+2));
        if (!color.isBlack() && !aColor.contains(color))
            aColor.append(SColor(color));
    }
    delete pixel;
}

void CView::changeCurrentMob(CMob *pMob)
{
    m_activeMob = pMob;
    emit updateMainWindowTitle(eTitleTypeData::eTitleTypeDataActiveMob, nullptr == pMob ? "" : pMob->mobName());

}

void CView::drawSelectFrame(QRect &rect)
{
    //convert frame to [(-1, 1), (-1, 1)]
    float leftX = rect.bottomLeft().x()/(float)m_width*2-1.0f;
    float leftY = rect.bottomLeft().y()/(float)m_height*2-1.0f;
    float rightX = rect.topRight().x()/(float)m_width*2-1.0f;
    float rightY = rect.topRight().y()/(float)m_height*2-1.0f;
    QPointF bottomLeft(leftX, -leftY);
    QPointF topRight(rightX, -rightY);
    m_selectFrame->updateFrame(bottomLeft, topRight);
}

void CView::pickObject(const QRect &rect, bool bAddToSelect)
{
    //TODO: use different buffer => save picking in image
    // Bind shader pipeline for use
    if(nullptr == m_activeMob)
        return;

    if (!m_selectProgram.bind())
        close();

    m_selectProgram.setUniformValue("u_projMmatrix", m_projection);
    m_selectProgram.setUniformValue("u_viewMmatrix", m_cam->update());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //if(m_operationType == EOperationTypeObjects)
    //for(const auto& mob : m_aMob)
    //foreach(const auto& mob, m_aMob)
    foreach (auto& node, CScene::getInstance()->getMode() == eEditModeLogic ? m_activeMob->logicNodes() : m_activeMob->nodes())
        node->drawSelect(&m_selectProgram);

    QVector<SColor> aColor;
    getColorFromRect(rect, aColor);

    //if(m_operationType == EOperationTypeObjects)

    if (!bAddToSelect) // clear selection buffer if we click out of objects in single selection mode
        m_activeMob->clearSelect(true);

    if (!aColor.isEmpty())
        foreach (auto& node, CScene::getInstance()->getMode() == eEditModeLogic ? m_activeMob->logicNodes() : m_activeMob->nodes())
            foreach (const auto& color, aColor)
            {
                if (node->isColorSuitable(color))
                {
                    if (bAddToSelect)
                    {//shift pressed

                        if (node->nodeState() & ENodeState::eSelect)
                        {//remove from select
                            node->setState(ENodeState::eDraw);
                        }
                        else
                        {// add to select
                            node->setState(ENodeState::eSelect);
                        }
                        break;
                    }
                    else
                    {//shift not pressed
                        node->setState(ENodeState::eSelect);
                        break;
                    }
                    break;
                }
            }

    m_selectFrame->reset();
}

void CView::loadMob(QFileInfo &filePath)
{
    m_pProgress->reset();
    CMob* pMob = new CMob;
    pMob->attach(this, m_pProgress);
    pMob->readMob(filePath);
    m_aMob.append(pMob);
    changeCurrentMob(pMob);
    emit mobLoad(false);
}

void CView::saveMobAs()
{
    if(nullptr == m_activeMob)
        return;

    const QFileInfo fileName = QFileDialog::getSaveFileName(this, "Save " + m_activeMob->mobName() + " as... ", "" , tr("Map objects (*.mob);;Mob as JSON(*.json)"));

    QSet<uint> aId;
    m_activeMob->checkUniqueId(aId);
    if (fileName.fileName().endsWith(".json"))
        m_activeMob->serializeJson(fileName);
    else if (fileName.fileName().endsWith(".mob"))
        m_activeMob->saveAs(fileName);

    m_activeMob->setFileName(fileName);
    emit updateMainWindowTitle(eTitleTypeData::eTitleTypeDataActiveMob, fileName.baseName());
    emit updateMainWindowTitle(eTitleTypeData::eTitleTypeDataDurtyFlag, "");
}

void CView::saveActiveMob()
{
    QSet<uint> aId;
    m_activeMob->checkUniqueId(aId);
    m_activeMob->save();
    emit updateMainWindowTitle(eTitleTypeData::eTitleTypeDataDurtyFlag, "");
}

void CView::saveAllMob()
{
    CMob* pMob = nullptr;
    QSet<uint> aId;
    foreach(pMob, m_aMob)
    {
        pMob->checkUniqueId(aId);
        pMob->save();
    }
    emit updateMainWindowTitle(eTitleTypeData::eTitleTypeDataDurtyFlag, "");
}

void CView::unloadMob(QString mobName)
{
    ei::log(eLogInfo, "unloading mob " + mobName);
    CMob* pMob = nullptr;
    if(mobName.isEmpty())
    {
        foreach(pMob, m_aMob)
            delete pMob;
        m_aMob.clear();
    }
    else
    {
        foreach(pMob, m_aMob)
        {
            if (pMob->mobName().toLower() == mobName.toLower())
            {
                delete pMob;
                m_aMob.removeOne(pMob);
                break;
            }
        }
    }
    emit mobLoad(true);
    if(!m_aMob.isEmpty())
        changeCurrentMob(m_aMob.back());
    else
        changeCurrentMob(nullptr);

    ei::log(eLogInfo, "Mob unloaded");
    viewParameters();
}

int CView::cauntSelectedNodes()
{
    int n = 0;
    for(const auto& mob: m_aMob)
    {
        for(const auto& node: mob->nodes())
            if(node->nodeState() == ENodeState::eSelect)
                ++n;
    }
    return n;
}

void CView::updateParameter(EObjParam param)
{
    QString valueToUpdate("");
    QString value;
    for (const auto& mob : m_aMob)
        for (const auto& node : mob->nodes())
        {
            if (node->nodeState() != ENodeState::eSelect)
                continue;

            value = node->getParam(param);
            if(!valueToUpdate.isEmpty() && valueToUpdate != value)
            {
                valueToUpdate = "";
                break;
            }
            valueToUpdate = value; //same values will be copied every time
        }

    m_tableManager->updateParam(param, valueToUpdate);
}

void CView::viewParameters()
{
    if(nullptr == m_activeMob)
        return;

    QSet<ENodeType> aType;
    //find unique selected node types
    //CMob* pMob = nullptr;
    //foreach(pMob, m_aMob)
    CNode* pNode = nullptr;
    foreach(pNode, CScene::getInstance()->getMode() == eEditModeLogic ? m_activeMob->logicNodes() : m_activeMob->nodes())
    {
        if (pNode->nodeState() != ENodeState::eSelect)
            continue;

        aType.insert(pNode->nodeType());
    }

    if (aType.isEmpty())
    {//reset parameters if not selected objects
        m_tableManager->reset();
        return;
    }

    //define common type for parameter display
    int ttt(aType.toList()[0]);
    foreach (const ENodeType& type, aType)
        ttt &= type;
    const ENodeType commonType = ENodeType(ttt);

    QMap<EObjParam, QString> aParam;
    foreach(pNode, CScene::getInstance()->getMode() == eEditModeLogic ? m_activeMob->logicNodes() : m_activeMob->nodes())
    {
        if (pNode->nodeState() != ENodeState::eSelect)
            continue;

        if(CScene::getInstance()->getMode() == eEditModeLogic)
            pNode->collectlogicParams(aParam, commonType);
        else
            pNode->collectParams(aParam, commonType);
    }

    m_tableManager->setNewData(aParam);
}

void CView::onParamChange(SParam &param)
{
    if(nullptr == m_activeMob)
        return;

    CNode* pNode = nullptr;
    foreach (pNode, CScene::getInstance()->getMode() == eEditModeLogic ? m_activeMob->logicNodes() : m_activeMob->nodes())
    {
        if (pNode->nodeState() != ENodeState::eSelect)
            continue;

        switch (param.param) {

        case eObjParam_ROTATION: //todo: need recalc landscape position for objects?
        case eObjParam_COMPLECTION:
        {
            CChangeStringParam* pChanger = new CChangeStringParam(this, pNode->mapId(), param.param, param.value);
            QObject::connect(pChanger, SIGNAL(updateParam()), this, SLOT(viewParameters()));
            QObject::connect(pChanger, SIGNAL(updatePosOnLand(CNode*)), this, SLOT(landPositionUpdate(CNode*)));
            m_pUndoStack->push(pChanger);
            break;
        }
        case eObjParam_TEMPLATE:
        {
            CChangeModelParam* pChanger = new CChangeModelParam(this, pNode->mapId(), param.param, param.value);
            QObject::connect(pChanger, SIGNAL(updateParam()), this, SLOT(viewParameters()));
            QObject::connect(pChanger, SIGNAL(updatePosOnLand(CNode*)), this, SLOT(landPositionUpdate(CNode*)));
            m_pUndoStack->push(pChanger);
            break;
        }
        default:
        {
            if(CScene::getInstance()->getMode() == eEditModeLogic)
            {
                CChangeLogicParam* pChanger = new CChangeLogicParam(this, pNode, param.param, param.value);
                QObject::connect(pChanger, SIGNAL(updateParam()), this, SLOT(viewParameters()));
                m_pUndoStack->push(pChanger);
            }
            else
            {
                CChangeStringParam* pChanger = new CChangeStringParam(this, pNode->mapId(), param.param, param.value);
                QObject::connect(pChanger, SIGNAL(updateParam()), this, SLOT(viewParameters()));
                m_pUndoStack->push(pChanger);
            }
            break;
        }
        }
    }

}

void CView::updateTreeLogic()
{
    if(nullptr == m_activeMob)
        return;

    m_pTree->clear();
    m_pTree->setColumnCount(2);
    QStringList labels;
    labels << "Point" << "Value";
    m_pTree->setHeaderLabels(labels);
    m_pTree->header()->setStretchLastSection(false);
    m_pTree->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    //m_pTree->resizeColumnToContents();
    CNode* pNode = nullptr;

    foreach(pNode, m_activeMob->nodes())
    {
        if(pNode->nodeState() != ENodeState::eSelect)
            continue;
        if(pNode->nodeType() != ENodeType::eUnit)
            continue;

        QJsonObject obj = pNode->toJson();
        QJsonObject logic = obj["Logics"].toArray()[0].toObject();
        EBehaviourType behaviour = (EBehaviourType)logic["Model"].toVariant().toInt();
        if(behaviour != EBehaviourType::ePath)
        {
            qDebug() << "behaviour: " << behaviour;
            continue;
        }
        //guard placement can be shown in table
        //QJsonArray arrPoint = logic["Guard placement"].toArray();
        //QVector3D pos(arrPoint[0].toVariant().toFloat(), arrPoint[1].toVariant().toFloat(), arrPoint[2].toVariant().toFloat());

        QJsonArray arrPatrol = logic["Patrol Points"].toArray();
        for(int i(0); i<arrPatrol.size(); ++i)
        {
            auto pPoint = new QTreeWidgetItem(m_pTree);
            QJsonArray arrPos = arrPatrol[i].toObject()["Position"].toArray();
            QVector3D pos(arrPos[0].toVariant().toFloat(), arrPos[1].toVariant().toFloat(), arrPos[2].toVariant().toFloat());

            pPoint->setText(0, util::makeString(pos));
            m_pTree->addTopLevelItem(pPoint);
            //look point
            QJsonArray arrLook = arrPatrol[i].toObject()["Looking points"].toArray();
            pPoint->setText(1, "views: " + QString::number(arrLook.size()));
            for(auto look = arrLook.begin(); look != arrLook.end(); ++look)
            {
                auto pView = new QTreeWidgetItem;
                QJsonArray arrPos = look->toObject()["Position"].toArray();
                QVector3D pos(arrPos[0].toVariant().toFloat(), arrPos[1].toVariant().toFloat(), arrPos[2].toVariant().toFloat());
                pView->setText(0, util::makeString(pos));
                uint wait = look->toObject()["Wait"].toVariant().toUInt();
                pView->setText(1, "time:" + QString::number(wait));
                pPoint->addChild(pView);
            }
        }
    }

    m_pTree->resizeColumnToContents(1);
    m_pTree->setColumnWidth(1, m_pTree->columnWidth(1)+10); // set intend for displaying
}

void CView::mousePressEvent(QMouseEvent* event)
{
    m_pOp->mousePressEvent(event);
}

void CView::mouseMoveEvent(QMouseEvent* event)
{
    m_pOp->mouseMoveEvent(event);
}

void CView::mouseReleaseEvent(QMouseEvent *event)
{
    m_pOp->mouseReleaseEvent(event);
}

void CView::wheelEvent(QWheelEvent* event)
{
    m_cam->enlarge(event->delta() > 0);
}

// draw objects without light and textures, only colored triangles; find suitable object
// x,y - mouse position
CNode* CView::pickObject(QList<CNode*>& aNode, int x, int y)
{
    // Bind shader pipeline for use
    if (!m_selectProgram.bind())
        close();

    m_selectProgram.setUniformValue("u_projMmatrix", m_projection);
    m_selectProgram.setUniformValue("u_viewMmatrix", m_cam->update());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (auto node: aNode)
    {
        node->drawSelect(&m_selectProgram);
    }

    GLubyte pixel[3];
    glReadPixels(x, m_height-y, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, pixel);
    SColor pickedColor(pixel[0], pixel[1], pixel[2]);

    for (auto& node: aNode)
    {
        if (node->isColorSuitable(pickedColor))
            return node;
    }

    return nullptr;
}

//clear buffer, draw only landscape, project point and return vector3D
QVector3D CView::getLandPos(const int cursorPosX, const int cursorPosY)
{
    QMatrix4x4 camMatrix = m_cam->viewMatrix();
    // Set modelview-projection matrix

    // Bind shader pipeline for use
    if (!m_landProgram.bind())
        close();

    // Set modelview-projection matrix
    m_landProgram.setUniformValue("u_projMmatrix", m_projection);
    m_landProgram.setUniformValue("u_viewMmatrix", camMatrix);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    auto pLand = CLandscape::getInstance();
    if (pLand && pLand->isMprLoad())
        pLand->draw(&m_landProgram);

    const int posY (height() - cursorPosY);
    float z;
    glReadPixels(cursorPosX, posY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &z );
    QVector3D point (cursorPosX, posY, z);

    QMatrix4x4 view = m_cam->viewMatrix();
    GLint viewPort[4];
    glGetIntegerv(GL_VIEWPORT, viewPort);
    QRect viewPortRect(viewPort[0], viewPort[1], viewPort[2], viewPort[3]);
    return point.unproject(view, m_projection, viewPortRect); //return Vector3D here
}

void CView::changeOperation(EButtonOp type)
{
    switch (type) {
    case EButtonOpSelect:
        m_pOp->changeState(new CSelect(this)); break;
    case EButtonOpMove:
        m_pOp->changeState(new CMoveAxis(this, EOperateAxisXY)); break;
    case EButtonOpRotate:
        m_pOp->changeState(new CRotateAxis(this, EOperateAxisZ)); break;
    case EButtonOpScale:
        m_pOp->changeState(new CScaleAxis(this, EOperateAxisZ)); break;
    }
}

void CView::operationSetBackup(EOperationAxisType operationType)
{
    m_operationBackup.clear();
    //for(const auto& mob : m_aMob)
        for (auto& node : CScene::getInstance()->getMode() == eEditModeLogic ? m_activeMob->logicNodes() : m_activeMob->nodes())
        {
            if (node->nodeState() != ENodeState::eSelect)
                continue;

            switch (operationType)
            {
            case EOperationAxisType::eMove:
                m_operationBackup[node] = node->position(); break;
            case EOperationAxisType::eRotate:
                m_operationBackup[node] = node->getEulerRotation(); break;
            case EOperationAxisType::eScale:
                m_operationBackup[node] = node->constitution(); break;
            }

        }
}

void CView::operationRevert(EOperationAxisType operationType)
{
    QQuaternion quat;
    if(m_operationBackup.isEmpty())
        return;

    for(auto& pair : m_operationBackup.toStdMap())
        switch (operationType)
        {
        case EOperationAxisType::eMove:
        {
            pair.first->updatePos(pair.second);
            break;
        }
        case EOperationAxisType::eRotate:
        {
            quat = QQuaternion::fromEulerAngles(pair.second);
            pair.first->setRot(quat);
            break;
        }
        case EOperationAxisType::eScale:
            pair.first->setConstitution(pair.second); break;
        }

}

void CView::operationApply(EOperationAxisType operationType)
{
//    if(CScene::getInstance()->getMode() == eEditModeLogic)
//    {
//        if(operationType == EOperationAxisType::eMove)
//        {
//            for(auto& pair : m_operationBackup.toStdMap())
//            {
//                CPatrolPoint* pPoint = dynamic_cast<CPatrolPoint*>(pair.first);
//                if(nullptr != pPoint)
//                {
//                    CNode* pParent = m_activeMob->findUnitParent(pPoint);

//                }
//                CChangeStringParam* pOp = new CChangeStringParam(this, pair.first->mapId(), EObjParam::eObjParam_POSITION, util::makeString(pair.first->position()));
//                QObject::connect(pOp, SIGNAL(updateParam()), this, SLOT(viewParameters()));
//                //pair.first->setPos(m_operationBackup[pair.first]);
//                pair.first->updatePos(m_operationBackup[pair.first]);
//                m_pUndoStack->push(pOp);
//            }
//        }
//    }
    QQuaternion quat;
    QVector3D rot;
    for(auto& pair : m_operationBackup.toStdMap())
    {
        switch (operationType)
        {
        case EOperationAxisType::eMove:
        {
            if(CScene::getInstance()->getMode() == eEditModeLogic && (pair.first->nodeType() == ePatrolPoint || pair.first->nodeType() == eLookPoint))
            {
                CChangeLogicParam* pOp = new CChangeLogicParam(this, pair.first, EObjParam::eObjParam_POSITION, util::makeString(pair.first->position()));
                QObject::connect(pOp, SIGNAL(updateParam()), this, SLOT(viewParameters()));
                pair.first->updatePos(m_operationBackup[pair.first]); //revert position to start (it will apply in 'push' operation
                m_pUndoStack->push(pOp);
            }
            else
            {
                CChangeStringParam* pOp = new CChangeStringParam(this, pair.first->mapId(), EObjParam::eObjParam_POSITION, util::makeString(pair.first->position()));
                QObject::connect(pOp, SIGNAL(updateParam()), this, SLOT(viewParameters()));
                pair.first->updatePos(m_operationBackup[pair.first]);
                m_pUndoStack->push(pOp);
            }

            break;
        }
        case EOperationAxisType::eRotate:
        {
            rot = pair.first->getEulerRotation();
            CChangeStringParam* pOp = new CChangeStringParam(this, pair.first->mapId(), EObjParam::eObjParam_ROTATION, util::makeString(rot));
            QObject::connect(pOp, SIGNAL(updateParam()), this, SLOT(viewParameters()));
            quat = QQuaternion::fromEulerAngles(m_operationBackup[pair.first]);
            pair.first->setRot(quat);
            m_pUndoStack->push(pOp);
            break;
        }
        case EOperationAxisType::eScale:
        {
            CChangeStringParam* pOp = new CChangeStringParam(this, pair.first->mapId(), EObjParam::eObjParam_COMPLECTION, util::makeString(pair.first->constitution()));
            QObject::connect(pOp, SIGNAL(updateParam()), this, SLOT(viewParameters()));
            pair.first->setConstitution(m_operationBackup[pair.first]);
            m_pUndoStack->push(pOp);
            break;
        }
        }

    }
    m_operationBackup.clear();
}

void CView::moveTo(QVector3D &dir)
{
    QVector3D pos;
    //for(const auto& mob : m_aMob)
    {
        for (auto& node : CScene::getInstance()->getMode() == eEditModeLogic ? m_activeMob->logicNodes() : m_activeMob->nodes())
        {
            if (node->nodeState() == ENodeState::eSelect)
            {
                pos = node->position() + dir;
                node->updatePos(pos);
            }
        }
    }
    updateParameter(EObjParam::eObjParam_POSITION);
}

void CView::rotateTo(QVector3D &rot)
{
    const float gimbalAvoidStep = 0.001f;
    QVector3D rotation; //this rotation will be tested
    QQuaternion quat; //this rotation as quat be applied to object
    QVector3D eulerRot; // temp variable
    const auto setNotNan = [&quat, &eulerRot, &gimbalAvoidStep](QVector3D& ch_rot)
    {
        for(int i(0); i< 100; ++i)
        {
            quat = QQuaternion::fromEulerAngles(ch_rot);
            eulerRot = quat.toEulerAngles();
            if(eulerRot.x() != eulerRot.x())
            {
                ch_rot.setX(ch_rot.x() + gimbalAvoidStep);
            }
            else if(eulerRot.y() != eulerRot.y())
            {
                ch_rot.setY(ch_rot.y() + gimbalAvoidStep);
            }
            else if(eulerRot.z() != eulerRot.z())
            {
                ch_rot.setZ(ch_rot.z() + gimbalAvoidStep);
            }
            else
                return;
        }
        return;
    };

    for(const auto& mob : m_aMob)
    {
        for (auto& node : mob->nodes())
        {
            if (node->nodeState() == ENodeState::eSelect)
            {
                rotation = node->getEulerRotation() + rot;
                setNotNan(rotation);
                node->setRot(quat);
            }
        }
    }
    updateParameter(EObjParam::eObjParam_ROTATION);
}

void CView::scaleTo(QVector3D &scale)
{
    QVector3D constitution;
    CMob* pMob = nullptr;
    foreach(pMob, m_aMob)
    {
        for (auto& node : pMob->nodes())
        {
            if (node->nodeState() == ENodeState::eSelect)
            {
                constitution = node->constitution() + scale;
                node->setConstitution(constitution);
            }
        }
    }
    updateParameter(EObjParam::eObjParam_COMPLECTION);
}

void CView::deleteSelectedNodes()
{
    QVector<uint> arrMapId;
    CNode* pNode = nullptr;
    foreach(pNode, CScene::getInstance()->getMode()==eEditModeLogic ? m_activeMob->logicNodes() : m_activeMob->nodes())
    {
        if (pNode->nodeState() != ENodeState::eSelect)
            continue;

        if(pNode->nodeType() == ePatrolPoint || pNode->nodeType() == eLookPoint)
        {
            CDeletePatrol* pUndo = new CDeletePatrol(pNode);
            m_pUndoStack->push(pUndo);
        }
        else
        {
            arrMapId.append(pNode->mapId());
        }
    }

    for(auto& id : arrMapId)
    {
        CDeleteNodeCommand* pUndo = new CDeleteNodeCommand(this, id);
        m_pUndoStack->push(pUndo);
    }

    viewParameters();
}

void CView::selectedObjectToClipboardBuffer()
{
    CNode* pNode;

    QJsonArray arrObj;
    //for(auto& mob: m_aMob)
    foreach(pNode, m_activeMob->nodes())
        if (pNode->nodeState() & ENodeState::eSelect)
            arrObj.append(pNode->toJson());

    QJsonObject obj;
    obj.insert("Data", arrObj);
    obj.insert("Magic", 1);
    QJsonDocument doc(obj);

    if (!m_clipboard_buffer_file.open(QIODevice::WriteOnly))
    {
        Q_ASSERT("Couldn't open option file." && false);
    }
    else
    {
        m_clipboard_buffer_file.write(doc.toJson(QJsonDocument::JsonFormat::Indented));
        m_clipboard_buffer_file.close();
    }
}

void CView::clipboradObjectsToScene()
{
    if (!m_clipboard_buffer_file.open(QIODevice::ReadOnly))
    {
        Q_ASSERT("Couldn't open option file." && false);
        return;
    }

    if (m_clipboard_buffer_file.size() == 0)
    {
        qDebug() << "empty copypasteBuffer file";
        return;
    }

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(m_clipboard_buffer_file.readAll(), &parseError);
    m_clipboard_buffer_file.close();
    //todo: check if has no error
    QJsonObject obj = doc.object();
    if (obj["Magic"].toInt() != 1)
        return;

    auto array = obj["Data"].toArray();
    if(array.isEmpty())
    {
        ei::log(eLogInfo, "clipboard empty");
        return;
    }

    m_activeMob->clearSelect();
    for(auto it = array.begin(); it < array.end(); ++it)
    {
        CCreateNodeCommand* pUndo = new CCreateNodeCommand(this, it->toObject());
        m_pUndoStack->push(pUndo);
    }
    viewParameters();
}

void CView::hideSelectedNodes()
{
    CMob* pMob = nullptr;
    CNode* pNode = nullptr;
    foreach (pMob, m_aMob)
    {
        foreach(pNode, pMob->nodes())
            if(pNode->nodeState() == ENodeState::eSelect)
                pNode->setState(ENodeState::eHidden);
    }
    viewParameters();
}

void CView::unHideAll()
{
    CMob* pMob = nullptr;
    CNode* pNode = nullptr;
    foreach (pMob, m_aMob)
    {
        foreach(pNode, pMob->nodes())
        if(pNode->nodeState() == ENodeState::eHidden)
            pNode->setState(ENodeState::eDraw);
    }

}

void CView::setDurty()
{
    emit updateMainWindowTitle(eTitleTypeData::eTitleTypeDataDurtyFlag, "*");
}

void CView::resetCamPosition()
{
    m_cam->reset();
}

void CView::addPatrolPoint()
{
    if(nullptr == m_activeMob)
        return;

    bool bChangeToMove = false;

    CNode* pNode = nullptr;
    foreach(pNode, m_activeMob->logicNodes())
    {
        if(pNode->nodeState() != ENodeState::eSelect)
            continue;

        if(pNode->nodeType() == ePatrolPoint)
        {
            CCreatePatrolCommand* pUndo = new CCreatePatrolCommand(this, dynamic_cast<CPatrolPoint*>(pNode));
            m_pUndoStack->push(pUndo);
            bChangeToMove = true;
        }
        else if(pNode->nodeType() == eUnit)
        {
            auto pUnit = dynamic_cast<CUnit*>(pNode);
            if(!pUnit->isBehaviourPath())
            {
                ei::log(eLogInfo, "object has no Path behaviour. adding patrol point skipped");
                continue; //skip different behaviour
            }

            CCreateUnitPatrolCommand* pUndo = new CCreateUnitPatrolCommand(this, pUnit);
            m_pUndoStack->push(pUndo);
            bChangeToMove = true;
        }
        pNode->setState(ENodeState::eDraw);

    }

    if(bChangeToMove)
        m_pOp->changeState(new CMoveAxis(this, EOperateAxisXY));
}
