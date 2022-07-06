#include <QtWidgets>
#include <QDebug>
#include <QtMath>       //for fabs
#include <QMap>         //for parameters
#include <QAbstractScrollArea>
#include <QUndoStack>

#include "view.h"
#include "camera.h"
#include "mob.h"
#include "texturelist.h"
#include "objectlist.h"
#include "node.h"
#include "landscape.h"
#include "key_manager.h"
#include "log.h"
#include "settings.h"
#include "table_item.h"
#include "undo.h"
#include "objects/object_base.h"
#include "objects/worldobj.h"
#include "objects/unit.h"
#include "mobparameters.h"
#include "progressview.h"
#include "tablemanager.h"
#include "operationmanager.h"
#include "selectframe.h"

class CLogic;

CView::CView(QWidget* parent):
    QGLWidget (parent)
    , m_landscape(nullptr)
    , m_objList(nullptr)
    , m_textureList(nullptr)
    , m_pSettings(nullptr)
    , m_pProgress(nullptr)
    , m_operationType(EOperationTypeObjects)
    , m_clipboard_buffer_file(QString("%1%2%3").arg(QDir::tempPath()).arg(QDir::separator()).arg("copy_paste_buffer.json"))
{
    setFocusPolicy(Qt::ClickFocus);

    m_cam.reset(new CCamera);
    m_timer = new QTimer;
    m_aReadState.resize(eReadCount);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(updateWindow()));
    m_operationBackup.clear();
    m_selectFrame.reset(new CSelectFrame);
}


void CView::updateReadState(EReadState state)
{
    m_aReadState[state] = true;
    qDebug() << state << " read";
}

CView::~CView()
{
    if (m_landscape)
        m_landscape->~CLandscape();

    for(auto& mob: m_aMob)
        delete mob;
}

CTextureList* CView::texList()
{
    Q_ASSERT(m_textureList); // for correctly update textures
    return m_textureList.get();
}
CObjectList* CView::objList()
{
    Q_ASSERT(m_objList); // for correctly update figures
    return m_objList.get();
}


void CView::attach(CSettings* pSettings, QTableWidget* pParam, QUndoStack* pStack, CProgressView* pProgress, QLineEdit* pMouseCoord) //todo: use signal\slots
{
    m_pSettings = pSettings;
    m_cam->attachSettings(pSettings);
    m_cam->reset();
    m_tableManager.reset(new CTableManager(pParam));
    QObject::connect(m_tableManager.get(), SIGNAL(changeParamSignal(SParam&)), this, SLOT(onParamChange(SParam&)));
    m_pUndoStack = pStack;

    //init object list
    m_objList.reset(new CObjectList);
    m_objList->attachSettings(m_pSettings);
    m_pProgress = pProgress;
    m_pOp.reset(new COperation(new CSelect(this)));
    m_pOp->attachCam(m_cam.get());
    m_cam->attachKeyManager(m_pOp->keyManager());
    m_pOp->attachkMouseCoordFiled(pMouseCoord);
    CLogger::getInstance()->attachSettings(pSettings);
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
    qglClearColor(Qt::black);
    //glEnable(GL_EXT_texture_filter_anisotropic);
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE); //draw only front faces (disable if need draw both sides)

    glEnable(GL_ALPHA_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    initShaders();
    //loadResource();
}

void CView::resizeGL(int width, int height)
{
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
}

void CView::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    draw();
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
    if (m_landscape)
        m_landscape->draw(&m_landProgram);

    // Bind shader pipeline for use
    if (!m_program.bind())
        close();

    m_program.setUniformValue("u_projMmatrix", m_projection);
    m_program.setUniformValue("u_viewMmatrix", camMatrix);

    for(auto& mob: m_aMob)
        for (auto& node: mob->nodes())
            node->draw(&m_program);

    if (m_landscape)
    {
        COptBool* pOpt = dynamic_cast<COptBool*>(settings()->opt("drawWater"));
        if (pOpt and pOpt->value() == true)
        {
            //turn to landshader again
            if (!m_landProgram.bind())
                close();

            m_landProgram.setUniformValue("transparency", 0.3f);
            m_landscape->drawWater(&m_landProgram);
            m_landProgram.setUniformValue("transparency", 0.0f);
        }
    }

    //draw selection frame using m_program shader
    if (!m_program.bind())
        close();

    if (m_landscape)
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
    if(m_landscape)
    {
        QMessageBox::warning(this, "Warning","Landscape already loaded. Please close before opening new zone (*mpr)");
        //LOG_FATAL("ahtung"); //test logging critial error
        return;
    }
    if(m_textureList.isNull())
    {
        m_textureList.reset(new CTextureList);
        m_textureList->attachSettings(m_pSettings);
    }

    ei::log(eLogInfo, "Start read landscape");
    m_landscape = new CLandscape;
    m_landscape->setParentView(this);
    m_landscape->readMap(filePath);
    ei::log(eLogInfo, "End read landscape");
    m_timer->setInterval(15); //"fps" for drawing
    m_timer->start();
}

void CView::unloadLand()
{
    if(m_landscape)
        delete m_landscape;

    m_landscape = nullptr;
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

    const auto h = qAbs(inversedRect.height());
    const auto w = qAbs(inversedRect.width());
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
    if (!m_selectProgram.bind())
        close();

    m_selectProgram.setUniformValue("u_projMmatrix", m_projection);
    m_selectProgram.setUniformValue("u_viewMmatrix", m_cam->update());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if(m_operationType == EOperationTypeObjects)
        //for(const auto& mob : m_aMob)
        foreach(const auto& mob, m_aMob)
            foreach (auto& node, mob->nodes())
                node->drawSelect(&m_selectProgram);

    QVector<SColor> aColor;
    getColorFromRect(rect, aColor);

    if(m_operationType == EOperationTypeObjects)
        foreach(const auto& mob, m_aMob)
        {
            if (!bAddToSelect) // clear selection buffer if we click out of objects in single selection mode
                mob->clearSelect();
            foreach (auto& node, mob->nodes())
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
        }
    m_selectFrame->reset();
}

void CView::loadMob(QFileInfo &filePath)
{
    m_pProgress->reset();
    CMob* mob = new CMob;
    mob->attach(this, m_pProgress);
    mob->readMob(filePath);
    m_aMob.append(mob);

    // update position on the map for each node
    Q_ASSERT(m_landscape);
    m_landscape->projectPositions(mob->nodes());
    emit mobLoad(false);
}

void CView::saveMobAs()
{
    QSet<uint> aId;
    for(auto& mob: m_aMob)
    {
        const QFileInfo fileName = QFileDialog::getSaveFileName(this, "Save " + mob->mobName() + " as... ", "" , tr("Map objects (*.mob);;Mob as JSON(*.json)"));
        if (fileName.fileName().endsWith(".json"))
        {
            mob->checkUniqueId(aId);
            mob->serializeJson(fileName);
        }
        else if (fileName.fileName().endsWith(".mob"))
        {
            mob->checkUniqueId(aId);
            mob->saveAs(fileName);
        }
    }
}

void CView::saveAllMob()
{
    QSet<uint> aId;
    for(const auto& mob: m_aMob)
    {
        mob->checkUniqueId(aId);
        mob->save();
    }
}

void CView::unloadMob(QString mobName)
{
    if(mobName.isEmpty())
    {
        for(auto mob: m_aMob)
            delete mob;
        m_aMob.clear();
    }
    else
    {
        for(auto mob: m_aMob)
        {
            if (mob->mobName().toLower() == mobName.toLower())
            {
                delete mob;
                m_aMob.removeOne(mob);
                break;
            }
        }
    }
    emit mobLoad(true);
    ei::log(eLogInfo, "Mob(s) unloaded " + mobName);
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

void CView::viewParameters()
{
    QSet<ENodeType> aType;
    //find unique selected node types
    for (const auto& mob : m_aMob)
        for (const auto& node : mob->nodes())
        {
            if (node->nodeState() != ENodeState::eSelect)
                continue;

            aType.insert(node->nodeType());
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
    for (const auto& mob : m_aMob)
        for (const auto& node : mob->nodes())
        {
            if (node->nodeState() != ENodeState::eSelect)
                continue;

            node->collectParams(aParam, commonType);
        }

    m_tableManager->setNewData(aParam);
}

void CView::onParamChange(SParam &param)
{
    for (auto& mob : m_aMob)
    {
        for (auto& node : mob->nodes())
        {
            if (node->nodeState() != ENodeState::eSelect)
                continue;
            switch (param.param) {

            case eObjParam_ROTATION: //todo: need recalc landscape position for objects?
            case eObjParam_COMPLECTION:
            {
                CChangeStringParam* pChanger = new CChangeStringParam(node, param.param, param.value);
                QObject::connect(pChanger, SIGNAL(updateParam()), this, SLOT(viewParameters()));
                QObject::connect(pChanger, SIGNAL(updatePosOnLand(CNode*)), this, SLOT(landPositionUpdate(CNode*)));
                m_pUndoStack->push(pChanger);
                //m_landscape->projectPosition(node);
                break;
            }
            case eObjParam_TEMPLATE:
            {
                CChangeModelParam* pChanger = new CChangeModelParam(node, param.param, param.value);
                QObject::connect(pChanger, SIGNAL(updateParam()), this, SLOT(viewParameters()));
                QObject::connect(pChanger, SIGNAL(updatePosOnLand(CNode*)), this, SLOT(landPositionUpdate(CNode*)));
                m_pUndoStack->push(pChanger);
                //m_landscape->projectPosition(node);
                break;
            }
            default:
            {
                CChangeStringParam* pChanger = new CChangeStringParam(node, param.param, param.value);
                QObject::connect(pChanger, SIGNAL(updateParam()), this, SLOT(viewParameters()));
                m_pUndoStack->push(pChanger);
                break;
            }
            }
        }
    }
}

void CView::landPositionUpdate(CNode *pNode)
{
    m_landscape->projectPosition(pNode);
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
    if (m_landscape)
        m_landscape->draw(&m_landProgram);

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
    for(const auto& mob : m_aMob)
        for (auto& node : mob->nodes())
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
    QQuaternion quat;
    QVector3D rot;
    for(auto& pair : m_operationBackup.toStdMap())
    {
        switch (operationType)
        {
        case EOperationAxisType::eMove:
        {
            CChangeStringParam* op = new CChangeStringParam(pair.first, EObjParam::eObjParam_POSITION, util::makeString(pair.first->position()));
            //pair.first->setPos(m_operationBackup[pair.first]);
            pair.first->updatePos(m_operationBackup[pair.first]);
            m_pUndoStack->push(op);
            break;
        }
        case EOperationAxisType::eRotate:
        {
            rot = pair.first->getEulerRotation();
            CChangeStringParam* op = new CChangeStringParam(pair.first, EObjParam::eObjParam_ROTATION, util::makeString(rot));
            quat = QQuaternion::fromEulerAngles(m_operationBackup[pair.first]);
            pair.first->setRot(quat);
            m_pUndoStack->push(op);
            break;
        }
        case EOperationAxisType::eScale:
            CChangeStringParam* op = new CChangeStringParam(pair.first, EObjParam::eObjParam_COMPLECTION, util::makeString(pair.first->constitution()));
            pair.first->setConstitution(m_operationBackup[pair.first]);
            m_pUndoStack->push(op);
            break;
        }

    }
    m_operationBackup.clear();
    viewParameters();
}

void CView::moveTo(QVector3D &dir)
{
    QVector3D pos;
    for(const auto& mob : m_aMob)
    {
        for (auto& node : mob->nodes())
        {
            if (node->nodeState() == ENodeState::eSelect)
            {
                pos = node->position() + dir;
                node->updatePos(pos);
            }
        }
    }
    viewParameters();
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
    viewParameters();
}

void CView::scaleTo(QVector3D &scale)
{
    QVector3D constitution;
    for(const auto& mob : m_aMob)
    {
        for (auto& node : mob->nodes())
        {
            if (node->nodeState() == ENodeState::eSelect)
            {
                constitution = node->constitution() + scale;
                node->setConstitution(constitution);
            }
        }
    }
    viewParameters();
}

void CView::deleteSelectedNodes()
{
    QVector<CNode*> aNode;
    CMob* mob = nullptr;
    foreach(mob, m_aMob)
    {
        aNode.clear();
        for (auto& node : mob->nodes())
            if (node->nodeState() == ENodeState::eSelect)
                aNode.append(node);

        for(auto& nd : aNode)
        {
            CDeleteNodeCommand* pUndo = new CDeleteNodeCommand(mob, nd);
            m_pUndoStack->push(pUndo);
        }
    }
    viewParameters();
}

void CView::selectedObjectToClipboardBuffer()
{
    CNode* pNode;

    QJsonArray arrObj;
    for(auto& mob: m_aMob)
        foreach(pNode, mob->nodes())
            if (pNode->nodeState() & ENodeState::eSelect)
                arrObj.append(pNode->toJson());

    QJsonDocument doc(arrObj);

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
