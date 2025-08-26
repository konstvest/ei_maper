#include <QtWidgets>
#include <QDebug>
#include <QtMath>       //for fabs
#include <QMap>         //for parameters
#include <QAbstractScrollArea>
#include <QUndoStack>
#include <QTreeWidget>
#include <QPair>
#include <QRandomGenerator>
#include <QMessageBox>

#include "view.h"
#include "camera.h"
#include "mob\mob.h"
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
#include "objects/magictrap.h"
#include "mob\mob_parameters.h"
#include "layout_components/progressview.h"
#include "layout_components/tablemanager.h"
#include "operationmanager.h"
#include "ogl_utils.h"
#include "scene.h"
#include "round_mob_form.h"
#include "layout_components/tree_view.h"
#include "property.h"
#include "tile.h"

class CLogic;

CView::CView(QWidget *parent, const QGLWidget *pShareWidget) : QGLWidget(parent, pShareWidget)
  ,m_pSettings(nullptr)
  ,m_pProgress(nullptr)
  ,m_clipboard_buffer_file(QString("%1%2%3").arg(QDir::tempPath()).arg(QDir::separator()).arg("copy_paste_buffer.json"))
  ,m_recentOpenedFile_file(QString("%1%2%3").arg(QDir::tempPath()).arg(QDir::separator()).arg("recent_opened.json"))
  ,m_activeMob(nullptr)
  ,m_pTree(nullptr)
  ,m_pRoundForm(nullptr)
  ,m_bDrawLand(true)
  ,m_bDrawWater(true)
  ,m_bPreviewTile(false)
  ,m_tileBrushCommandId(0)
{
    setFocusPolicy(Qt::ClickFocus);

    m_cam.reset(new CCamera);
    m_timer = new QTimer;
    m_aReadState.resize(eReadCount);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(updateWindow()));
    m_mprModifyTimer = new QTimer;
    connect(m_mprModifyTimer, SIGNAL(timeout()), this, SLOT(checkNewLandVersion()));
    m_operationBackup.clear();
    m_selectFrame.reset(new CSelectFrame);
    //qDebug() << format();
    //qDebug() << isSharing();
    QApplication* application = static_cast<QApplication *>(QApplication::instance());
    application->inputMethod()->reset();
    QLocale a = application->inputMethod()->locale();
    QString lang = a.languageToString(a.language());
    a.setDefault(QLocale::English);
    m_pLand = CLandscape::getInstance();
}

void CView::updateReadState(EReadState state)
{
    m_aReadState[state] = true;
    qDebug() << state << " read";
}

CView::~CView()
{
//    for(auto& param : m_arrParamWindow)
//    {
//        param->close();
//        delete param;
//    }

    for(auto& mob: m_aMob)
        delete mob;
}


void CView::attach(CSettings* pSettings, QTableWidget* pParam, QUndoStack* pStack, CProgressView* pProgress, QLineEdit* pMouseCoord, CTreeView *pTree, CTileForm* pTileForm) //todo: use signal\slots
{
    m_pSettings = pSettings;
    m_cam->attachSettings(pSettings);
    m_cam->reset();
    m_tableManager.reset(new CTableManager(pParam));
    QObject::connect(m_tableManager.get(), SIGNAL(onUpdateProperty(const QSharedPointer<IPropertyBase>)), this, SLOT(onParamChange(const QSharedPointer<IPropertyBase>&)));
    m_pUndoStack = pStack;

    m_pProgress = pProgress;
    m_pOp.reset(new COperation(new CSelect(this)));
    m_pOp->attachCam(m_cam.get());
    m_cam->attachKeyManager(m_pOp->keyManager());
    m_pOp->attachMouseCoordFiled(pMouseCoord);
    CLogger::getInstance()->attachSettings(pSettings);

    m_pTree = pTree;
    m_pTree->attachView(this);
    m_pTileForm = pTileForm;
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
    //logOpenGlData();
    //qDebug() << QOpenGLContext::openGLModuleType();
    makeCurrent();
    qglClearColor(Qt::black);
    //glEnable(GL_EXT_texture_filter_anisotropic);
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE); //draw only front faces (disable if need draw both sides)
    glAlphaFunc(GL_GREATER, 0.1f);
    glEnable(GL_ALPHA_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    initShaders();
    checkOpenGlError();

    CTextureList::getInstance()->initResource();
    CObjectList::getInstance()->initResource();
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
    if (m_pLand && m_pLand->isMprLoad() && m_bPreviewTile) // draw preview tile on top
        drawTilePreview(&m_landProgram);

    if (m_pLand && m_pLand->isMprLoad() && m_bDrawLand)
        m_pLand->draw(&m_landProgram);

    // Bind shader pipeline for use
    if (!m_program.bind())
        close();

    m_program.setUniformValue("u_projMmatrix", m_projection);
    m_program.setUniformValue("u_viewMmatrix", camMatrix);

    if(m_activeMob != nullptr)
    {
        for(auto& mob: m_aMob) //TODO: draw inactive mobs with custom color|alfa or something
        {
            if(mob->mobName() == m_activeMob->mobName())
                continue; //skip drawing active mob

            for (auto& node: mob->nodes())
                node->draw(false, &m_program);
        }

        for (auto& node: m_activeMob->nodes())
            node->draw(true, &m_program);
    }

    if (m_pLand && m_pLand->isMprLoad() && m_bDrawWater)
    {
        //turn to landshader again
        if (!m_landProgram.bind())
            close();

        m_landProgram.setUniformValue("transparency", 0.3f);
        m_pLand->drawWater(&m_landProgram);
        m_landProgram.setUniformValue("transparency", 0.0f);
    }


    //draw selection frame using m_program shader
    if (!m_program.bind())
        close();

    if (m_pLand && m_pLand->isMprLoad() && !m_aMob.isEmpty())
    {
        QMatrix4x4 mtrx;
        mtrx.setToIdentity();
        m_program.setUniformValue("u_projMmatrix", mtrx);
        m_program.setUniformValue("u_viewMmatrix", mtrx);
        m_selectFrame->draw(&m_program);
    }
}

void CView::loadLandscape(const QFileInfo& filePath)
{
    if(m_pLand->isMprLoad())
    {
        QMessageBox::warning(this, "Warning","Landscape already loaded. Please close before opening new zone (*mpr)");
        //LOG_FATAL("ahtung"); //test logging critial error
        return;
    }

    m_pLand->readMap(filePath);
    emit updateMainWindowTitle(eTitleTypeData::eTitleTypeDataMpr, filePath.baseName());
    emit updateMainWindowTitle(eTitleTypeData::eTitleTypeDataMprDirtyFlag, m_pLand->isDirty() ? "*" : "");
    m_timer->setInterval(15); //"fps" for drawing
    m_timer->start();
    m_lastModifiedLand = filePath.lastModified();
    connect(m_pTileForm, SIGNAL(applyChangesSignal()), this, SLOT(onMapMaterialUpdate()));
    COptInt* pOpt = dynamic_cast<COptInt*>(settings()->opt("landCheckTime"));
    if (pOpt and pOpt->value() != 0)
    {
        m_mprModifyTimer->setInterval(pOpt->value());
        m_mprModifyTimer->start();
    }
    m_pPreviewTile.reset(new CPreviewTile());
    updateTileForm(true);
}

void CView::unloadLand()
{
    if(!m_pLand->isMprLoad())
        return;

    if(m_pLand->isDirty())
    {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Unload MPR", m_pLand->filePath().baseName() + " has unsaved changes.\nDo you want to save changes?", QMessageBox::Save|QMessageBox::No|QMessageBox::Cancel);
        if(reply == QMessageBox::Save)
            m_pLand->save();
        else if(reply == QMessageBox::Cancel)
            return;

    }

    m_mprModifyTimer->stop();
    m_pLand->unloadMpr();
    emit updateMainWindowTitle(eTitleTypeData::eTitleTypeDataMpr, "");
    m_pPreviewTile.clear();
    ei::log(eLogInfo, "Landscape unloaded");
}

void CView::saveLand()
{
    if(m_pLand->isMprLoad())
    {
        m_pLand->save();
        emit updateMainWindowTitle(eTitleTypeData::eTitleTypeDataMprDirtyFlag, "");
    }
}

void CView::saveLandAs()
{
    if(!m_pLand->isMprLoad())
        return;

    const QFileInfo fileName = QFileDialog::getSaveFileName(this, "Save " + m_pLand->filePath().fileName() + " as... ", "" , tr("Landscape (*.mpr);;)"));
    //QFileInfo fileName("c:\\konst\\Проклятые Земли (Дополнение)\\Mods\\ferneo_mod\\Maps\\zone1gTest.mpr");

    m_pLand->saveMapAs(fileName);
    emit updateMainWindowTitle(eTitleTypeData::eTitleTypeDataMpr, fileName.baseName());
    emit updateMainWindowTitle(eTitleTypeData::eTitleTypeDataMprDirtyFlag, "");
}

int CView::select(const SSelect &selectParam, bool bAddToSelect)
{
    if(nullptr == m_activeMob)
        return 0;

    CNode* pNode = nullptr;
    foreach (pNode, m_activeMob->nodes())
    {
        switch (selectParam.type) {
        case eSelectType_Id_range:
        {
            uint id_min = selectParam.param1.toUInt();
            uint id_max = selectParam.param2.toUInt();
            // set equal value if only one defined
            if(id_min!=0 && id_max==0)
                id_max = id_min;
            else if(id_max!=0 && id_min==0)
                id_min = id_max;

            if (id_max < id_min)
            {
                uint temp = id_min;
                id_min = id_max;
                id_max = temp;
            }
            if (pNode->mapId() >= id_min && pNode->mapId() <=id_max)
            {
                pNode->setState(eSelect);
            }
            else if (!bAddToSelect && (pNode->nodeState() & eSelect)) //deselect
                pNode->setState(eDraw);
            break;
        }
        case eSelectType_Map_name:
        {
            auto pObj = dynamic_cast<CObjectBase*>(pNode);
            if (!pObj)
                break;
            bool bAppropriate = selectParam.exactMatch ? pNode->mapName() == selectParam.param1 : pNode->mapName().toLower().contains(selectParam.param1.toLower());
            if (bAppropriate)
            {
                pNode->setState(eSelect);
            }
            else if (!bAddToSelect && (pNode->nodeState() & eSelect)) //deselect
                pNode->setState(eDraw);
            break;
        }
        case eSelectType_Texture_name:
        {
            if (!(pNode->nodeType() & ENodeType::eWorldObject))
                break;
            bool bAppropriate = selectParam.exactMatch ? pNode->textureName() == selectParam.param1 : pNode->textureName().toLower().contains(selectParam.param1.toLower());
            if (bAppropriate)
            {
                pNode->setState(eSelect);
            }
            else if (!bAddToSelect && (pNode->nodeState() & eSelect)) //deselect
                pNode->setState(eDraw);
            break;
        }
        case eSelectType_Model_name:
        {
            if (!(pNode->nodeType() & ENodeType::eWorldObject))
                break;

            bool bAppropriate = selectParam.exactMatch ? pNode->modelName() == selectParam.param1 : pNode->modelName().toLower().contains(selectParam.param1.toLower());
            if (bAppropriate)
            {
                pNode->setState(eSelect);
            }
            else if (!bAddToSelect && (pNode->nodeState() & eSelect)) //deselect
                pNode->setState(eDraw);
            break;
        }
        case eSelectType_Position_circle:
        {
            //TODO. create 'brush' for selecti by circle
            break;
        }
        case eSelectType_Position_rectangle:
        {
            //TODO. create 'brush' for selecti by circle
            break;
        }
        case eSelectType_Diplomacy_group:
        {
            auto pObj = dynamic_cast<CWorldObj*>(pNode);
            if (!pObj)
            {//deselect if we are trying to object diplomacy group with selected particles(and others)
                if(!bAddToSelect && (pNode->nodeState() & eSelect))
                    pNode->setState(eDraw);
                break;
            }

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
                pNode->setState(eSelect);
            }
            else if (!bAddToSelect && (pNode->nodeState() & eSelect)) //deselect
            {
                pNode->setState(eDraw);
            }
            break;
        }
        case eSelectType_Database_name:
        {
            auto pObj = dynamic_cast<CUnit*>(pNode);
            if (!pObj)
            { //deselect if we are trying to find unit database name with selected world object(and others)
                if(!bAddToSelect && (pNode->nodeState() & eSelect))
                    pNode->setState(eDraw);
                break;
            }

            bool bAppropriate = selectParam.exactMatch ? pObj->databaseName() == selectParam.param1 : pObj->databaseName().toLower().contains(selectParam.param1.toLower());
            if (bAppropriate)
            {
                pNode->setState(eSelect);
            }
            else if (!bAddToSelect && (pNode->nodeState() & eSelect)) //deselect
            {
                pNode->setState(eDraw);
            }
            break;
        }
        case eSelectType_Template:
        {
            auto pWo = dynamic_cast<CWorldObj*>(pNode);
            if(nullptr == pWo)
                break;

            QSharedPointer<IPropertyBase> templ;
            pWo->getParam(templ, eObjParam_PARENT_TEMPLATE);
            bool bAppropriate = selectParam.exactMatch ? templ->toString() == selectParam.param1 : templ->toString().toLower().contains(selectParam.param1.toLower());
            if(bAppropriate)
                pNode->setState(ENodeState::eSelect);
            else if (!bAddToSelect && (pNode->nodeState() & eSelect)) //deselect
            {
                pNode->setState(eDraw);
            }

            break;
        }
        case eSelectType_all:
        {
            if(selectParam.param1.toLower()=="logic")
            {
                if(m_activeMob->logicNodes().contains(pNode))
                    pNode->setState(eSelect);
                else
                    pNode->setState(eDraw);
            }
            else
                pNode->setState(eSelect);
            break;
        }
        default:
            break;
        }
    }
    viewParameters();
    return nSelectedNodes();
}

CMob *CView::mob(QString mobName)
{
    CMob* pMob = nullptr;
    foreach(pMob, m_aMob)
    {
        if(pMob->filePath().absoluteFilePath().toLower() == mobName) //try to find fullpath name first
            break;

        if (pMob->mobName().toLower() == mobName.toLower())
            break;
    }

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
    emit updateMainWindowTitle(eTitleTypeData::eTitleTypeDataDirtyFlag, (nullptr == pMob || !pMob->isDirty()) ? "" : "*");
    collectObjectTreeData();
}

void CView::changeCurrentMob(QString mobName)
{
    if(nullptr != m_activeMob)
        m_activeMob->clearSelect();

    for(auto& mob : m_aMob)
    {
        if(mob->mobName() == mobName)
        {
            changeCurrentMob(mob);
            break;
        }

    }
}

void CView::onParamChangeLogic(CNode *pNode, const QSharedPointer<IPropertyBase>& prop)
{
    ENodeType type = pNode->nodeType();
    switch(type)
    {
    case eMagicTrap:
    {
        CChangeLogicParam* pChanger = new CChangeLogicParam(this, QString::number(pNode->mapId()), prop);
        QObject::connect(pChanger, SIGNAL(updateParam()), this, SLOT(viewParameters()));
        m_pUndoStack->push(pChanger);
        break;
    }
    case eUnit:
    {
        CChangeLogicParam* pChanger = new CChangeLogicParam(this, QString::number(pNode->mapId()), prop);
        QObject::connect(pChanger, SIGNAL(updateParam()), this, SLOT(viewParameters()));
        m_pUndoStack->push(pChanger);
        break;
    }
    case ePatrolPoint:
    {
        auto pPoint = dynamic_cast<CPatrolPoint*>(pNode);
        int unitId(0);
        int patrolId(0);
        m_activeMob->getPatrolHash(unitId, patrolId, pPoint);
        QString hash = QString("%1.%2").arg(unitId).arg(patrolId);
        CChangeLogicParam* pOp = new CChangeLogicParam(this, hash, prop);
        QObject::connect(pOp, SIGNAL(updateParam()), this, SLOT(viewParameters()));
        m_pUndoStack->push(pOp);
        break;
    }
    case eLookPoint:
    {
        auto pPoint = dynamic_cast<CLookPoint*>(pNode);
        int unitId(0);
        int patrolId(0);
        int viewId(0);
        m_activeMob->getViewHash(unitId, patrolId, viewId, pPoint);
        QString hash = QString("%1.%2.%3").arg(unitId).arg(patrolId).arg(viewId);
        CChangeLogicParam* pOp = new CChangeLogicParam(this, hash, prop);
        QObject::connect(pOp, SIGNAL(updateParam()), this, SLOT(viewParameters()));
        m_pUndoStack->push(pOp);
        break;
    }
    case eTrapActZone:
    {
        auto pZone = dynamic_cast<CActivationZone*>(pNode);
        int unitId(0);
        int patrolId(-2);
        int viewId(-2);
        int zoneId(0);
        m_activeMob->getTrapZoneHash(unitId, zoneId, pZone);
        QString hash = QString("%1.%2.%3.%4").arg(unitId).arg(patrolId).arg(viewId).arg(zoneId);
        CChangeLogicParam* pOp = new CChangeLogicParam(this, hash, prop);
        QObject::connect(pOp, SIGNAL(updateParam()), this, SLOT(viewParameters()));
        m_pUndoStack->push(pOp);
        break;
    }
    case eTrapCastPoint:
    {
        auto pCast = dynamic_cast<CTrapCastPoint*>(pNode);
        int unitId(0);
        int patrolId(-2);
        int viewId(-2);
        int zoneId(-2);
        int castPointId(0);
        m_activeMob->getTrapCastHash(unitId, castPointId, pCast);
        QString hash = QString("%1.%2.%3.%4.%5").arg(unitId).arg(patrolId).arg(viewId).arg(zoneId).arg(castPointId);
        CChangeLogicParam* pOp = new CChangeLogicParam(this, hash, prop);
        QObject::connect(pOp, SIGNAL(updateParam()), this, SLOT(viewParameters()));
        m_pUndoStack->push(pOp);
        break;
    }
    default:
    {
        Q_ASSERT(false && "not implemented");
        break;
    }
    }
}

QString stringFromUnsignedChar(const unsigned char *str ){
    QString result = "";
    int lengthOfString = strlen( reinterpret_cast<const char*>(str) );

    // print string in reverse order
    QString s;
    for( int i = 0; i < lengthOfString; i++ ){
        s = QString( "%1" ).arg( str[i], 0, 16 );

        // account for single-digit hex values (always must serialize as two digits)
        if( s.length() == 1 )
            result.append( "0" );

        result.append( s );
    }

    return result;
}

void CView::logOpenGlData()
{
    const GLubyte * version = glGetString(GL_VERSION);
    ei::log(eLogInfo, (char*)version);
    const GLubyte * funcs = glGetString(GL_EXTENSIONS);
    ei::log(eLogInfo, (char*)funcs);
}

void CView::checkOpenGlError()
{
    GLenum errCode;
    errCode = glGetError();
    if (errCode != GL_NO_ERROR)
    {
        ei::log(eLogFatal, "OpenGL error: " + QString::number(errCode));

    }
}

void CView::drawTilePreview(QOpenGLShaderProgram* program)
{
    if(m_pPreviewTile.isNull())
        return;

    m_pLand->glTexture()->bind(0); // todo: update preview tile texture after mpr loaded
    program->setUniformValue("qt_Texture0", 0);
    m_pPreviewTile->draw(program);
}

void CView::updateTileForm(bool bGenerateNewTable)
{
    if(bGenerateNewTable)
        m_pTileForm->fillTable(m_pLand->mapName(), m_pLand->textureNum());
    m_pTileForm->setTileTypes(m_pLand->tileTypes());
    m_pTileForm->setMaterial(m_pLand->materials());
    m_pTileForm->setAnimTile(m_pLand->animTiles());
}

void CView::onChangeCursorTile(QPixmap ico)
{
    if(m_pOp->operationMethod() != eOperationMethodTileBrush)
        return;
    // Создаем базовое изображение для курсора (например, простой квадрат)
    QPixmap baseCursorPixmap(42, 42);
    baseCursorPixmap.fill(Qt::transparent);  // Прозрачный фон

    QPainter painter(&baseCursorPixmap);
    painter.setRenderHint(QPainter::Antialiasing);

    // Устанавливаем цвет и толщину пера для стрелки
    QPen pen(CScene::getInstance()->isLandTileEditMode() ? Qt::red : Qt::green);
    pen.setWidth(2);
    painter.setPen(pen);

    // Рисуем стрелку в левом верхнем углу
    painter.drawLine(2, 2, 10, 10);   // Линия тела стрелки
    painter.drawLine(2, 2, 2, 6);    // Линия нижней части стрелки
    painter.drawLine(2, 2, 6, 2);    // Линия верхней части стрелки

    painter.end();

    // Загружаем дополнительное изображение, которое нужно добавить к курсору
    //QPixmap additionalPixmap(":/images/icon.png");

    // Совмещаем оба изображения: рисуем дополнительное поверх базового
    QPixmap combinedPixmap(baseCursorPixmap.size());
    combinedPixmap.fill(Qt::transparent);

    painter.begin(&combinedPixmap);
    painter.drawPixmap(0, 0, baseCursorPixmap);  // Базовый курсор
    painter.drawPixmap(10, 10, ico); // Дополнительное изображение
    painter.end();

    // Создаем новый курсор из объединенного изображения
    QCursor customCursor(combinedPixmap, 0, 0);

    // Устанавливаем новый курсор для виджета
    setCursor(customCursor);

//    QCursor curs(ico, 0, 0);
    //    setCursor(curs);
}

void CView::onMapMaterialUpdate()
{
    if(!m_pLand->isMprLoad())
        return;

    m_pLand->setMaterials(m_pTileForm->material());
    m_pLand->setAnimTils(m_pTileForm->animTile());
    m_pLand->setTileTypes(m_pTileForm->tileTypes());
}

void CView::onRestoreTileData()
{

}

void CView::checkNewLandVersion()
{
    return; //todo: set option to auto-check changes in mpr. Dont re-upload when save by ourselfes
    // Dont use QFileSystemWatcher because it spams signals several time for 1 MapEd save
    QFileInfo landPath(m_pLand->filePath());
    landPath.refresh();
    QDateTime lastM = landPath.lastModified();
    if (lastM > m_lastModifiedLand)
    {
        unloadLand();
        loadLandscape(landPath);
        m_lastModifiedLand = lastM;
    }
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
    {
        m_selectFrame->reset();
        return;
    }

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

QVector3D CView::getTerrainPos(bool bLand)
{
    QPoint globalCursorPos = QCursor::pos();
    QPoint localCursorPos = mapFromGlobal(globalCursorPos);
    return getTerrainPos(localCursorPos.x(), localCursorPos.y(), bLand);
}

void CView::loadMob(QFileInfo &filePath)
{
    m_pProgress->reset();
    CMob* pMob = new CMob;
    pMob->attach(this, m_pProgress);
    pMob->readMob(filePath);
    m_aMob.append(pMob);
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
    emit updateMainWindowTitle(eTitleTypeData::eTitleTypeDataDirtyFlag, "");
}

void CView::saveActiveMob()
{
    if(nullptr == m_activeMob)
        return;

    QSet<uint> aId;
    m_activeMob->checkUniqueId(aId);
    m_activeMob->save();
    m_activeMob->setDirty(false);
    emit updateMainWindowTitle(eTitleTypeData::eTitleTypeDataDirtyFlag, "");
}

void CView::saveAllMob()
{
    CMob* pMob = nullptr;
    QSet<uint> aId;
    foreach(pMob, m_aMob)
    {
        pMob->checkUniqueId(aId);
        pMob->save();
        pMob->setDirty(false);
    }
    emit updateMainWindowTitle(eTitleTypeData::eTitleTypeDataDirtyFlag, "");
}

void CView::unloadActiveMob()
{
    if(nullptr == m_activeMob)
        return;

    ei::log(eLogInfo, "unloading mob " + m_activeMob->mobName());
    CMob* pMob = nullptr;
    foreach(pMob, m_aMob)
    {
        if(m_activeMob != pMob)
            continue;

        if(pMob->isDirty())
        {
            QMessageBox::StandardButton reply;
            reply = QMessageBox::question(this, "Unload MOB", pMob->mobName() + " has unsaved changes.\nDo you want to save changes?", QMessageBox::Save|QMessageBox::No|QMessageBox::Cancel);
            if(reply == QMessageBox::Save)
                pMob->save();
            else if(reply == QMessageBox::Cancel)
                return;

        }
        m_aMob.removeOne(pMob);
        emit unloadMob(pMob);
        delete pMob;
        break;
    }

    if(!m_aMob.isEmpty())
        changeCurrentMob(m_aMob.back());
    else
        changeCurrentMob(nullptr);

    ei::log(eLogInfo, "Mob unloaded");
    viewParameters();
}

void CView::openActiveMobEditParams()
{
    if(nullptr == m_activeMob)
        return;

    CMobParameters* pParamWindow = nullptr;

    foreach(pParamWindow, m_arrParamWindow)
    {
        if(m_activeMob == pParamWindow->mob())
        {
            qDebug() << "mob already opened";
            return;
        }
    }

    auto pParam = new CMobParameters(nullptr, m_activeMob, this);
    QObject::connect(this, SIGNAL(unloadMob(CMob*)), pParam, SLOT(onMobUnload(CMob*)));
    QObject::connect(pParam, SIGNAL(editFinishedSignal(CMobParameters*)), this, SLOT(onMobParamEditFinished(CMobParameters*)));
    m_arrParamWindow.append(pParam);
    pParam->show();
}

void CView::unloadMob(QString mobName)
{
    if(m_aMob.isEmpty())
        return;

    ei::log(eLogInfo, "unloading mob " + mobName);
    CMob* pMob = nullptr;
    if(mobName.isEmpty())
    {
        foreach(pMob, m_aMob)
        {
            emit unloadMob(pMob);
            delete pMob;
        }
        m_aMob.clear();
        m_activeMob = nullptr;
    }
    else
    {
        foreach(pMob, m_aMob)
        {
            if (pMob->mobName().toLower() == mobName.toLower())
            {
                emit unloadMob(pMob);
                delete pMob;
                m_aMob.removeOne(pMob);
                break;
            }
        }
    }

    if(!m_aMob.isEmpty())
        changeCurrentMob(m_aMob.back());
    else
        changeCurrentMob(nullptr);

    ei::log(eLogInfo, "Mob unloaded");
    viewParameters();
}

int CView::nSelectedNodes()
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

QList<CNode *> CView::selectedNodes()
{
    QList<CNode*> arrNode;
    for(const auto& mob: m_aMob)
    {
        for(const auto& node: mob->nodes())
            if(node->nodeState() == ENodeState::eSelect)
                arrNode.append(node);
    }
    return arrNode;
}

void CView::setRandomComplection(const EObjParam param, const float min, const float max)
{
    if(nullptr == m_activeMob)
        return;

    CNode* pNode = nullptr;
    foreach(pNode, m_activeMob->nodes())
    {
        if (pNode->nodeState() != ENodeState::eSelect)
            continue;

        float value = util::randomFloat(min ,max);
        QSharedPointer<propFloat> complexity(new propFloat(param, value));
        CChangeProp* pChanger = new CChangeProp(this, pNode->mapId(), complexity);
        QObject::connect(pChanger, SIGNAL(updateParam()), this, SLOT(viewParameters()));
        //QObject::connect(pChanger, SIGNAL(updatePosOnLand(CNode*)), this, SLOT(landPositionUpdate(CNode*)));
        m_pUndoStack->push(pChanger);
    }
    updateParameter(param);
}

void CView::resetSelectedId()
{
    if(nullptr == m_activeMob)
        return;

    // get MOB ranges - is a high priority. Chck if valid data (10+)
    QVector<SRange> arrRange = m_activeMob->ranges(!m_activeMob->isQuestMob());
    if(arrRange.isEmpty())
    {
        ei::log(eLogWarning, "not found available MOB ID range to reset IDs");
        QMessageBox::warning(this, "Warning", "Not found available MOB ID range to reset IDs. Check MOB parameters,");
        return;
    }
    for(auto& range: arrRange)
    {
        if(range.minRange < 10)
            range.minRange = 10;
        if(range.maxRange <= range.minRange)
        {
            ei::log(eLogWarning, "Incorrect MOB ranges:"+QString::number(range.minRange) + ":" + QString::number(range.maxRange));
            break;
        }
    }
    // collect busy IDs (skip ID from selected objects)
    QVector<uint> arrBusyId;
    QVector<uint> arrNodes;
    CNode* pNode = nullptr;
    foreach(pNode, m_activeMob->nodes())
    {
        if (pNode->nodeState() == ENodeState::eSelect)
            arrNodes.append(pNode->mapId());
        else
        {
            arrBusyId.append(pNode->mapId());
        }
    }
    if(arrNodes.isEmpty())
        return;
    // find range of free IDs
    std::sort(arrBusyId.begin(), arrBusyId.end());
    uint min;
    uint max;
    uint startId(0);
    const auto nElem = arrNodes.size();
    bool bFound = false;
    for(auto& range: arrRange)
    {
        if(nElem > int(range.maxRange-range.minRange))
            continue; // skip too small ranges
        min = range.minRange;
        max = min;
        for(int i(0); i<arrBusyId.size(); ++i)
        {
            if(arrBusyId[i]<min)
                continue; //filter left-side value
            if(arrBusyId[i]==min)
            {// filter conflict value
                ++min;
                max = min;
                continue;
            }
            if(arrBusyId[i]>=range.maxRange)
                break; //filter right-side value
            if(arrBusyId[i] > max)
            { // found value in range
                max = arrBusyId[i];
            }
            else
            {
                continue;
            }
            if(int(max-min) >= nElem)
            {
                startId = min;
                bFound = true;
                break;
            }
            else
            {
                min = arrBusyId[i]+1;
                max = min;
            }
        }
        if(bFound)
            break;
        //not found between busy ids
        if(int(range.maxRange - max) >= nElem)
        {
            startId = max;
            break;
        }
    }

    QMap<uint, uint> reconnectId;
    for(int i(0); i<nElem; ++i)
    {
        reconnectId[arrNodes[i]] = startId;
        ++startId;
    }
    for(auto pair: reconnectId.toStdMap())
        qDebug() << pair.first << ": " << pair.second;
    CResetIdCommand* pReset = new CResetIdCommand(this, reconnectId);
    QObject::connect(pReset, SIGNAL(updateParam()), this, SLOT(viewParameters()));
    m_pUndoStack->push(pReset);
    m_activeMob->setDirty();
    // update tree view?
}


void CView::openMapParameters()
{
    if(!m_pLand->isMprLoad())
        return;

    if(m_pTileForm->isVisible())
    {
        m_pTileForm->show();
        return; // dont update params if window already opened
    }

    updateTileForm();
    m_pTileForm->show();
}

void CView::pickTile(QVector3D posOnLand, bool bLand)
{
    if(!m_pLand->isMprLoad())
        return;

    CTile* pTile = nullptr;
    STileLocation tileLoc;
    if(!m_pLand->pickTile(posOnLand, pTile, tileLoc, bLand))
        return;

    m_pTileForm->selectTile(pTile->tileIndex());
    m_pTileForm->setTileRotation(pTile->tileRotation());
    if(!bLand)
        m_pTileForm->setActiveMatIndex(pTile->materialIndex());

    m_pPreviewTile->updateTile(*pTile, pTile->tileIndex(), pTile->tileRotation(), pTile->materialIndex(), tileLoc.xSec, tileLoc.ySec);
}

void CView::setTile(QVector3D posOnLand, bool bLand)
{
    if(!m_pLand->isMprLoad())
        return;

    CTile* pTile = nullptr;
    STileLocation tileLoc;
    if(!m_pLand->pickTile(posOnLand, pTile, tileLoc, bLand))
        return;

    int index, rotNum, matIndex;
    m_pTileForm->getSelectedTile(index, rotNum);
    if(index < 0)
        return;

    matIndex = m_pTileForm->activeMaterialindex();

    STileInfo tileInfoNew{index, rotNum, matIndex};
    STileInfo tileInfoOld{pTile->tileIndex(), pTile->tileRotation(), pTile->materialIndex()};
    CBrushTileCommand* pReset = new CBrushTileCommand(this, tileInfoNew, tileLoc, tileInfoOld, m_tileBrushCommandId);
    m_pUndoStack->push(pReset);

//    pTile->setTile(index, rotNum);
//    if(!bLand)
//        pTile->setMaterialIndex(matIndex);

//    m_pLand->updateSectorDrawData(tileLoc.xSec, tileLoc.ySec);
}

void CView::setTile(QMap<STileLocation, STileInfo>& arrTileData)
{
    if(!m_pLand->isMprLoad())
        return;

    m_pLand->setTile(arrTileData);
}

void CView::updatePreviewTile(QVector3D posOnLand, bool bLand)
{
    if(!m_pLand->isMprLoad())
        return;

    CTile* pTile = nullptr;
    STileLocation tileLoc;
    if(!m_pLand->pickTile(posOnLand, pTile, tileLoc, bLand))
        return;

    onChangeCursorTile(m_pTileForm->tileWithRot(pTile->tileIndex()));
    int index, rotNum, matIndex;
    m_pTileForm->getSelectedTile(index, rotNum);
    if(index < 0)
        return;

    matIndex = m_pTileForm->activeMaterialindex();
    m_pPreviewTile->updateTile(*pTile, index, rotNum, matIndex, tileLoc.xSec, tileLoc.ySec);
}

void CView::addTileRotation(int step)
{
    int curRot = (m_pTileForm->tileRotation() + step)%4;
    if(curRot < 0)
        curRot = 3;
    m_pTileForm->setTileRotation(curRot);
}

void CView::showOutliner(bool bShow)
{
    emit showOutlinerSignal(bShow);
}

void CView::pickQuickAccessTile(int index)
{
    if(!m_pLand->isMprLoad())
        return;

    m_pTileForm->selectQuickAccessTile(index);
}

void CView::endTileBrushGroup()
{
    m_tileBrushCommandId = (m_tileBrushCommandId+1)%9;
}

bool CView::onExit()
{
    bool bCloseAllowed = true;
    CMob* pMob = nullptr;
    foreach(pMob, m_aMob)
    {
        if(!pMob->isDirty())
            continue;

        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Exit", pMob->mobName() + " has unsaved changes.\nDo you want to save changes?", QMessageBox::Save|QMessageBox::No|QMessageBox::Cancel);
        if(reply == QMessageBox::Save)
            pMob->save();
        else if(reply == QMessageBox::Cancel)
        {
            bCloseAllowed = false;
            break;
        }
    }
    if(!bCloseAllowed)
        return bCloseAllowed;

    if(m_pLand && m_pLand->isDirty())
    {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Exit", m_pLand->filePath().baseName() + " has unsaved changes.\nDo you want to save changes?", QMessageBox::Save|QMessageBox::No|QMessageBox::Cancel);
        if(reply == QMessageBox::Save)
            m_pLand->save();
        else if(reply == QMessageBox::Cancel)
        {
            bCloseAllowed = false;
        }
    }
    if(bCloseAllowed)
    {
        saveSession();
    }
    return bCloseAllowed;
}

void CView::updateParameter(EObjParam propType)
{
    return; // not all prop types can be updated for each node. Use direct updating for each operation separately.

    if(nullptr == m_activeMob)
        return;

    QSharedPointer<IPropertyBase> valueToUpdate;
    QSharedPointer<IPropertyBase> value;
    for (const auto& node : CScene::getInstance()->getMode() == eEditModeLogic ? m_activeMob->logicNodes() : m_activeMob->nodes())
    {
        if (node->nodeState() != ENodeState::eSelect)
            continue;

        if(node->nodeType() == eTrapActZone || node->nodeType() == eTrapCastPoint || node->nodeType() == eLookPoint || node->nodeType() == ePatrolPoint)
            node->getLogicParam(value, propType);
        else
            node->getParam(value, propType);

        if(!valueToUpdate.isNull() && !valueToUpdate->isEqual(value.get()))
        {
            valueToUpdate.reset();
            break;
        }
        valueToUpdate.reset(value->clone()); //same values will be copied every time
    }
    //todo: pipe to direct update value ot x,y,z, in cell
    //m_tableManager->updateParam(param, valueToUpdate);
}

void CView::viewParameters()
{
    if(nullptr == m_activeMob)
    {
        m_tableManager->reset(); //clear table data if empty current mob
        return;
    }

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

    QList<QSharedPointer<IPropertyBase>> aProp;
    foreach(pNode, CScene::getInstance()->getMode() == eEditModeLogic ? m_activeMob->logicNodes() : m_activeMob->nodes())
    {
        if (pNode->nodeState() != ENodeState::eSelect)
            continue;

        if(CScene::getInstance()->getMode() == eEditModeLogic)
            pNode->collectlogicParams(aProp, commonType);
        else
            pNode->collectParams(aProp, commonType);
    }

    m_tableManager->setNewData(aProp);
}

void CView::onParamChange(const QSharedPointer<IPropertyBase>& prop)
{
    if(nullptr == m_activeMob)
        return;

    CNode* pNode = nullptr;
    foreach (pNode, CScene::getInstance()->getMode() == eEditModeLogic ? m_activeMob->logicNodes() : m_activeMob->nodes())
    {
        if (pNode->nodeState() != ENodeState::eSelect)
            continue;

        switch (prop->type()) {

        case eObjParam_ROTATION: //todo: need recalc landscape position for objects?
        case eObjParam_COMPLECTION:
        {
            CChangeProp* pChanger = new CChangeProp(this, pNode->mapId(), prop);
            QObject::connect(pChanger, SIGNAL(updateParam()), this, SLOT(viewParameters()));
            //QObject::connect(pChanger, SIGNAL(updatePosOnLand(CNode*)), this, SLOT(landPositionUpdate(CNode*)));
            m_pUndoStack->push(pChanger);
            break;
        }
        case eObjParam_TEMPLATE:
        {
            CChangeProp* pChanger = new CChangeProp(this, pNode->mapId(), prop); // #todo: update node pos after undo-redo
            QObject::connect(pChanger, SIGNAL(updateParam()), this, SLOT(viewParameters()));
            //QObject::connect(pChanger, SIGNAL(updatePosOnLand(CNode*)), this, SLOT(landPositionUpdate(CNode*)));
            m_pUndoStack->push(pChanger);
            break;
        }
        case eObjParam_NID:
        {
            if(nSelectedNodes() > 1)
            {
                ei::log(eLogWarning, "incorrect operation. You cannot set the same identifier for several objects");
                QMessageBox::warning(nullptr, "Operation Error", "incorrect operation. You cannot set the same identifier for several objects");
                return;
            }
            auto pId = dynamic_cast<propUint*>(prop.get());
            if(!m_activeMob->isFreeId(pId->value()))
            {
                ei::log(eLogWarning, "incorrect operation. Duplicate id");
                QMessageBox::warning(nullptr, "Operation Error", "incorrect operation. Duplicate id");
                return;
            }
            CChangeProp* pChanger = new CChangeProp(this, pNode->mapId(), prop);
            QObject::connect(pChanger, SIGNAL(updateParam()), this, SLOT(viewParameters()));
            QObject::connect(pChanger, SIGNAL(changeIdSignal(uint,uint)), m_pTree, SLOT(onChangeNodeId(uint,uint)));
            QObject::connect(pChanger, SIGNAL(changeTreeName(CNode*)), m_pTree, SLOT(onChangeObjectName(CNode*)));
            m_pUndoStack->push(pChanger);
            break;
        }
        default:
        {
            if(CScene::getInstance()->getMode() == eEditModeLogic)
            {
                onParamChangeLogic(pNode, prop);
            }
            else
            {
                CChangeProp* pChanger = new CChangeProp(this, pNode->mapId(), prop);
                QObject::connect(pChanger, SIGNAL(updateParam()), this, SLOT(viewParameters()));
                QObject::connect(pChanger, SIGNAL(changeIdSignal(uint,uint)), m_pTree, SLOT(onChangeNodeId(uint,uint)));
                QObject::connect(pChanger, SIGNAL(changeTreeName(CNode*)), m_pTree, SLOT(onChangeObjectName(CNode*)));
                m_pUndoStack->push(pChanger);
            }
            break;
        }
        }
    }

}



void CView::collectObjectTreeData()
{
    m_pTree->clear();
    if(nullptr == m_activeMob)
        return;

    m_pTree->setColumnCount(2);
    QStringList labels;
    labels << "Objects" << "Count";
    m_pTree->setHeaderLabels(labels);
    m_pTree->header()->setStretchLastSection(false);
    m_pTree->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    m_pTree->resizeColumnToContents(1);
    //generate common object types
    QMap<QString, QMap<QString, QList<uint>>> aObjList; //base group -> inner object group -> object string array (todo: item class with node map ID)
    CNode* pNode = nullptr;
    ENodeType type = ENodeType::eBaseType;
    QString groupName;
    auto fillObjectByType=[&aObjList, &type, &groupName, this](uint objId) -> void
    {
        QString typeString = m_pTree->objectNameByType(type);
        if(aObjList.contains(typeString))
        {
            auto& group = aObjList[typeString];
            group[groupName].append(objId);

        }
        else
        {
            QMap<QString, QList<uint>> map;
            map[groupName].append(objId);
            aObjList[typeString] = map;
        }
    };
    foreach(pNode, m_activeMob->nodes())
    {
        type = pNode->nodeType();
        //can be optimized via class overriding
        switch (type)
        {
        case ENodeType::eTorch:
        case ENodeType::eLever:
        case ENodeType::eMagicTrap:
        case ENodeType::eLight:
        case ENodeType::eSound:
        case ENodeType::eParticle:
        case ENodeType::eWorldObject:
        {
            groupName = pNode->mapName();
            fillObjectByType(pNode->mapId());
            break;
        }
        case ENodeType::eUnit:
        {
            auto pUnitItem = dynamic_cast<CUnit*>(pNode);
            groupName = pUnitItem->databaseName();
            fillObjectByType(pNode->mapId());
            break;
        }
        default:
        {
            //do not collect nothing for Tree View
            break;
        }

        }
    }

    for(auto& item : aObjList.toStdMap())
    {
        auto pTopItem = new CTreeObject(m_pTree);
        pTopItem->setText(0, item.first);
        m_pTree->addTopLevelItem(pTopItem);
        for(auto& group : item.second.toStdMap())
        {
            auto pGroupItem = new CTreeObject(pTopItem, 0);
            pGroupItem->setText(1, QString::number(group.second.size()));
            if(group.second.size() == 1)
            {
                // +"(id:"+QString::number(group.second.first())+")"
                pGroupItem->setText(0, group.first);
                pGroupItem->setId(group.second.front());
                continue;
            }
            pGroupItem->setText(0, group.first);
            for(auto& object : group.second)
            {
                auto pObjectItem = new CTreeObject(pGroupItem, object);
                pObjectItem->setText(0, QString::number(object));
            }
        }
    }
    m_pTree->sortItems(0, Qt::SortOrder::AscendingOrder);
}

void CView::onRestoreCursor()
{
    unsetCursor();
}

void CView::moveCamToSelectedObjects()
{
    CNode* pNode = nullptr;
    CBox box;
    foreach(pNode, m_activeMob->nodes())
    {
        if (pNode->nodeState() != ENodeState::eSelect)
            continue;
        // calculate world position
        CBox nodeBox = pNode->getBBox();
        nodeBox.move(pNode->drawPosition());
        box.expand(nodeBox);
    }

    if(box.isInit())
    {
        m_cam->moveTo(box.center());
        m_cam->moveAwayOn(box.radius()*2.5f); // too much for multi large objects
    }
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
    m_pOp->wheelEvent(event);
}

void CView::focusOutEvent(QFocusEvent *event)
{
    Q_UNUSED(event);
    auto keySet = m_pOp->keyManager()->keys();
    for(auto& key : keySet)
    {
        m_pOp->keyRelease(key);
    }
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
QVector3D CView::getTerrainPos(const int cursorPosX, const int cursorPosY, bool bLand)
{
    QVector3D point(0,0,0);
    if(!m_pLand || !m_pLand->isMprLoad())
        return point;

    QMatrix4x4 camMatrix = m_cam->viewMatrix();
    // Set modelview-projection matrix

    // Bind shader pipeline for use
    if (!m_landProgram.bind())
        close();

    // Set modelview-projection matrix
    m_landProgram.setUniformValue("u_projMmatrix", m_projection);
    m_landProgram.setUniformValue("u_viewMmatrix", camMatrix);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if(bLand)
        m_pLand->draw(&m_landProgram);
    else
    {
        m_pLand->drawWater(&m_landProgram);
    }
    const int posY (height() - cursorPosY);
    float z;
    glReadPixels(cursorPosX, posY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &z );
    //QVector3D point (cursorPosX, posY, z);
    point.setX(cursorPosX);
    point.setY(posY);
    point.setZ(z);

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
    case EButtonOpTilebrush:
        m_pOp->changeState(new CTileBrush(this)); break;
    }
}

void CView::operationSetBackup(EOperationAxisType operationType)
{
    if(nullptr == m_activeMob)
        return;

    m_operationBackup.clear();
    for (auto& node : CScene::getInstance()->getMode() == eEditModeLogic ? m_activeMob->logicNodes() : m_activeMob->nodes())
    {
        if (node->nodeState() != ENodeState::eSelect)
            continue;
        if(!node->isOperationAxisAllow(operationType))
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
            if(CScene::getInstance()->getMode() == eEditModeLogic)
            {
                ENodeType type = pair.first->nodeType();
                switch(type)
                {
                case eUnit:
                {
                    QSharedPointer<IPropertyBase> pos (new prop3D(eObjParam_POSITION, pair.first->position()));
                    CChangeProp* pOp = new CChangeProp(this, pair.first->mapId(), pos);
                    QObject::connect(pOp, SIGNAL(updateParam()), this, SLOT(viewParameters()));
                    pair.first->updatePos(m_operationBackup[pair.first]);
                    m_pUndoStack->push(pOp);
                    break;
                }
                case ePatrolPoint:
                {
                    auto pPoint = dynamic_cast<CPatrolPoint*>(pair.first);
                    int unitId(0);
                    int patrolId(0);
                    m_activeMob->getPatrolHash(unitId, patrolId, pPoint);
                    QString hash = QString("%1.%2").arg(unitId).arg(patrolId);
                    QSharedPointer<IPropertyBase> pos (new prop3D(eObjParam_POSITION, pair.first->position()));
                    CChangeLogicParam* pOp = new CChangeLogicParam(this, hash, pos);
                    QObject::connect(pOp, SIGNAL(updateParam()), this, SLOT(viewParameters()));
                    pair.first->updatePos(m_operationBackup[pair.first]); //revert position to start (it will apply in 'push' operation
                    m_pUndoStack->push(pOp);
                    break;
                }
                case eLookPoint:
                {
                    auto pPoint = dynamic_cast<CLookPoint*>(pair.first);
                    int unitId(0);
                    int patrolId(0);
                    int viewId(0);
                    m_activeMob->getViewHash(unitId, patrolId, viewId, pPoint);
                    QString hash = QString("%1.%2.%3").arg(unitId).arg(patrolId).arg(viewId);
                    QSharedPointer<IPropertyBase> pos (new prop3D(eObjParam_POSITION, pair.first->position()));
                    CChangeLogicParam* pOp = new CChangeLogicParam(this, hash, pos);
                    QObject::connect(pOp, SIGNAL(updateParam()), this, SLOT(viewParameters()));
                    pair.first->updatePos(m_operationBackup[pair.first]); //revert position to start (it will apply in 'push' operation
                    m_pUndoStack->push(pOp);
                    break;
                }
                case eTrapActZone:
                {
                    auto pZone = dynamic_cast<CActivationZone*>(pair.first);
                    int unitId(0);
                    int patrolId(-2);
                    int viewId(-2);
                    int zoneId(0);
                    m_activeMob->getTrapZoneHash(unitId, zoneId, pZone);
                    QString hash = QString("%1.%2.%3.%4").arg(unitId).arg(patrolId).arg(viewId).arg(zoneId);
                    QSharedPointer<IPropertyBase> pos (new prop3D(eObjParam_POSITION, pair.first->position()));
                    CChangeLogicParam* pOp = new CChangeLogicParam(this, hash, pos);
                    QObject::connect(pOp, SIGNAL(updateParam()), this, SLOT(viewParameters()));
                    pair.first->updatePos(m_operationBackup[pair.first]); //revert position to start (it will apply in 'push' operation
                    m_pUndoStack->push(pOp);
                    break;
                }
                case eTrapCastPoint:
                {
                    auto pCast = dynamic_cast<CTrapCastPoint*>(pair.first);
                    int unitId(0);
                    int patrolId(-2);
                    int viewId(-2);
                    int zoneId(-2);
                    int castPointId(0);
                    m_activeMob->getTrapCastHash(unitId, castPointId, pCast);
                    QString hash = QString("%1.%2.%3.%4.%5").arg(unitId).arg(patrolId).arg(viewId).arg(zoneId).arg(castPointId);
                    QSharedPointer<IPropertyBase> pos (new prop3D(eObjParam_POSITION, pair.first->position()));
                    CChangeLogicParam* pOp = new CChangeLogicParam(this, hash, pos);
                    QObject::connect(pOp, SIGNAL(updateParam()), this, SLOT(viewParameters()));
                    pair.first->updatePos(m_operationBackup[pair.first]); //revert position to start (it will apply in 'push' operation
                    m_pUndoStack->push(pOp);
                    break;
                }
                default:
                    break;
                }


            }
            else
            {
                QSharedPointer<IPropertyBase> pos (new prop3D(eObjParam_POSITION, pair.first->position()));
                CChangeProp* pOp = new CChangeProp(this, pair.first->mapId(), pos);
                QObject::connect(pOp, SIGNAL(updateParam()), this, SLOT(viewParameters()));
                pair.first->updatePos(m_operationBackup[pair.first]);
                m_pUndoStack->push(pOp);
            }

            break;
        }
        case EOperationAxisType::eRotate:
        {
            rot = pair.first->getEulerRotation();
            QSharedPointer<IPropertyBase> rotation (new prop3D(eObjParam_ROTATION, rot));
            CChangeProp* pOp = new CChangeProp(this, pair.first->mapId(), rotation);
            QObject::connect(pOp, SIGNAL(updateParam()), this, SLOT(viewParameters()));
            quat = QQuaternion::fromEulerAngles(m_operationBackup[pair.first]);
            pair.first->setRot(quat);
            m_pUndoStack->push(pOp);
            break;
        }
        case EOperationAxisType::eScale:
        {
            QSharedPointer<IPropertyBase> complection (new prop3D(eObjParam_COMPLECTION, pair.first->constitution()));
            CChangeProp* pOp = new CChangeProp(this, pair.first->mapId(), complection);
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
    if(nullptr == m_activeMob)
        return;

    QVector3D pos;
    for (auto& node : CScene::getInstance()->getMode() == eEditModeLogic ? m_activeMob->logicNodes() : m_activeMob->nodes())
    {
        if (node->nodeState() != ENodeState::eSelect)
            continue;

        pos = node->position() + dir;
        node->updatePos(pos);
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


    for(auto& node : m_operationBackup.keys())
    {
        rotation = node->getEulerRotation() + rot;
        setNotNan(rotation);
        node->setRot(quat);
    }

    updateParameter(EObjParam::eObjParam_ROTATION);
}

void CView::scaleTo(QVector3D &scale)
{
    if(nullptr == m_activeMob)
        return;

    QVector3D constitution;
    for (auto& node : m_activeMob->nodes())
    {
        if (node->nodeState() != ENodeState::eSelect)
            continue;

        constitution = node->constitution() + scale;
        node->setConstitution(constitution);
    }

    updateParameter(EObjParam::eObjParam_COMPLECTION);
}

void CView::resetUnitLogicPaths()
{
    if(nullptr == m_activeMob)
        return;

    //todo: make undo for this operation
    CNode* pNode = nullptr;
    foreach(pNode, m_activeMob->logicNodes())
    {
        if(pNode->nodeState() != ENodeState::eSelect)
            continue;

        if(pNode->nodeType() != ENodeType::eUnit)
            continue;

        auto pUnit = dynamic_cast<CUnit*>(pNode);
        pUnit->clearPaths();
    }
    m_activeMob->logicNodesUpdate();
    m_activeMob->setDirty();
}

void CView::deleteSelectedNodes()
{
    QVector<uint> arrMapId;
    CNode* pNode = nullptr;
    foreach(pNode, CScene::getInstance()->getMode()==eEditModeLogic ? m_activeMob->logicNodes() : m_activeMob->nodes())
    {
        if (pNode->nodeState() != ENodeState::eSelect)
            continue;

        auto type = pNode->nodeType();
        switch(type)
        {
        case ePatrolPoint:
        {
            auto pPoint = dynamic_cast<CPatrolPoint*>(pNode);
            int unitId(0);
            int patrolId(0);
            m_activeMob->getPatrolHash(unitId, patrolId, pPoint);
            QString hash = QString("%1.%2").arg(unitId).arg(patrolId);
            CDeleteLogicPoint* pOp = new CDeleteLogicPoint(this, hash);
            m_pUndoStack->push(pOp);
            break;
        }
        case eLookPoint:
        {
            auto pPoint = dynamic_cast<CLookPoint*>(pNode);
            int unitId(0);
            int patrolId(0);
            int viewId(0);
            m_activeMob->getViewHash(unitId, patrolId, viewId, pPoint);
            QString hash = QString("%1.%2.%3").arg(unitId).arg(patrolId).arg(viewId);
            CDeleteLogicPoint* pOp = new CDeleteLogicPoint(this, hash);
            m_pUndoStack->push(pOp);
            break;
        }
        case eTrapActZone:
        {
            auto pZone = dynamic_cast<CActivationZone*>(pNode);
            int unitId(0);
            int patrolId(-2);
            int viewId(-2);
            int zoneId(0);
            m_activeMob->getTrapZoneHash(unitId, zoneId, pZone);
            QString hash = QString("%1.%2.%3.%4").arg(unitId).arg(patrolId).arg(viewId).arg(zoneId);
            CDeleteLogicPoint* pOp = new CDeleteLogicPoint(this, hash);
            m_pUndoStack->push(pOp);
            break;
        }
        case eTrapCastPoint:
        {
            auto pCast = dynamic_cast<CTrapCastPoint*>(pNode);
            int unitId(0);
            int patrolId(-2);
            int viewId(-2);
            int zoneId(-2);
            int castPointId(0);
            m_activeMob->getTrapCastHash(unitId, castPointId, pCast);
            QString hash = QString("%1.%2.%3.%4.%5").arg(unitId).arg(patrolId).arg(viewId).arg(zoneId).arg(castPointId);
            CDeleteLogicPoint* pOp = new CDeleteLogicPoint(this, hash);
            m_pUndoStack->push(pOp);
            break;
        }
        default:
        {
            arrMapId.append(pNode->mapId());
            break;
        }
        }
    }

    for(auto& id : arrMapId)
    {
        CDeleteNodeCommand* pUndo = new CDeleteNodeCommand(this, id);
        QObject::connect(pUndo, SIGNAL(deleteNodeSignal(uint)), m_pTree, SLOT(onNodeDelete(uint)));
        QObject::connect(pUndo, SIGNAL(undo_deleteNodeSignal(CNode*)), m_pTree, SLOT(addNodeToTree(CNode*)));
        m_pUndoStack->push(pUndo);
    }

    viewParameters();
    //updateViewTree();
}

void CView::selectedObjectToClipboardBuffer()
{
    if(nullptr == m_activeMob)
        return;

    CNode* pNode;

    QJsonArray arrObj;
    //for(auto& mob: m_aMob)
    foreach(pNode, m_activeMob->nodes())
        if (pNode->nodeState() & ENodeState::eSelect)
            arrObj.append(pNode->toJson());

    QJsonObject obj;
    obj.insert("Version", 2);
    auto pos = QWidget::mapFromGlobal(QCursor::pos());
    auto posOnLand = getTerrainPos(pos.x(), pos.y());
    QJsonArray point;
    point.append(QJsonValue::fromVariant(posOnLand.x()));
    point.append(QJsonValue::fromVariant(posOnLand.y()));
    point.append(QJsonValue::fromVariant(posOnLand.z()));
    obj.insert("Mouse position", point);
    obj.insert("Data", arrObj);

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
    if (obj["Version"].toInt() != 2)
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
        QObject::connect(pUndo, SIGNAL(addNodeSignal(CNode*)), m_pTree, SLOT(addNodeToTree(CNode*)));
        QObject::connect(pUndo, SIGNAL(undo_addNodeSignal(uint)), m_pTree, SLOT(onNodeDelete(uint)));
        m_pUndoStack->push(pUndo);
    }
    //move copyed nodes to new mouse position
    QVector3D oldMousePos;
    QJsonArray arrPoint = obj["Mouse position"].toArray();
    if(arrPoint.size()==3)
        oldMousePos = QVector3D(arrPoint[0].toVariant().toFloat(), arrPoint[1].toVariant().toFloat(), arrPoint[2].toVariant().toFloat());

    auto pos = QWidget::mapFromGlobal(QCursor::pos());
    auto posOnLand = getTerrainPos(pos.x(), pos.y());
    auto mouseDif = posOnLand - oldMousePos;
    mouseDif.setZ(0.0f);
    m_pOp->changeState(new CMoveAxis(this, EOperateAxisXY));
    moveTo(mouseDif);

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

void CView::setDirtyMpr()
{
    m_pLand->setDirty(true);
    emit updateMainWindowTitle(eTitleTypeData::eTitleTypeDataMprDirtyFlag, "*");
}

void CView::setDurty(CMob* pMob)
{
    if(pMob && pMob != m_activeMob)
        pMob->setDirty();
    else
    {
        m_activeMob->setDirty(true);
        emit updateMainWindowTitle(eTitleTypeData::eTitleTypeDataDirtyFlag, "*");
    }
}

void CView::resetCamPosition()
{
    m_cam->reset();
}

void CView::addLogicPoint(bool bLookPoint)
{
    if(nullptr == m_activeMob)
        return;

    bool bChangeToMove = false;

    CNode* pNode = nullptr;
    foreach(pNode, m_activeMob->logicNodes())
    {
        if(pNode->nodeState() != ENodeState::eSelect)
            continue;
        ENodeType type = pNode->nodeType();
        switch(type)
        {
        case eLookPoint:
        {
            if(!bLookPoint)
                break; //dont create patrol point for view point

            int unitId(0); //parent unit map id
            int patrolId(0); //patrol parent id
            int viewId(0); //parent view id
            auto pLook = dynamic_cast<CLookPoint*>(pNode);
            m_activeMob->getViewHash(unitId, patrolId, viewId, pLook);
            QString hash = QString("%1.%2.%3").arg(unitId).arg(patrolId).arg(viewId);
            CCreatePatrolCommand* pUndo = new CCreatePatrolCommand(this, hash);
            m_pUndoStack->push(pUndo);
            bChangeToMove = true;
            break;
        }
        case ePatrolPoint:
        {
            int unitId(0); //parent unit map id
            int patrolId(0); //patrol parent id
            int viewId(-1); //parent view id
            auto pPoint = dynamic_cast<CPatrolPoint*>(pNode);
            QString hash;
            m_activeMob->getPatrolHash(unitId, patrolId, pPoint);
            if(bLookPoint)
                hash = QString("%1.%2.%3").arg(unitId).arg(patrolId).arg(viewId); // for creating first looking point
            else
                hash = QString("%1.%2").arg(unitId).arg(patrolId); // for creating patrol point

            CCreatePatrolCommand* pUndo = new CCreatePatrolCommand(this, hash);
            m_pUndoStack->push(pUndo);
            bChangeToMove = true;
            break;
        }
        case eUnit:
        {
            if(bLookPoint)
                break; //dont create look point for unit

            if(!dynamic_cast<CUnit*>(pNode)->isBehaviourPath())
            {
                ei::log(eLogInfo, "object has non-Path behaviour. adding patrol point skipped");
                break; //skip non-path behaviour
            }

            QString hash = QString("%1.%2").arg(pNode->mapId()).arg(-1);
            CCreatePatrolCommand* pUndo = new CCreatePatrolCommand(this, hash);
            m_pUndoStack->push(pUndo);
            bChangeToMove = true;
            break;
        }
        case eMagicTrap:
        {
            CCreateTrapPointCommand* pUndo = new CCreateTrapPointCommand(this, pNode->mapId(), !bLookPoint);
            m_pUndoStack->push(pUndo);
            bChangeToMove = true;
            break;
        }
        case eTrapActZone:
        {
            uint trapId = m_activeMob->trapIdByPoint(dynamic_cast<CActivationZone*>(pNode));
            CCreateTrapPointCommand* pUndo = new CCreateTrapPointCommand(this, trapId, !bLookPoint);
            m_pUndoStack->push(pUndo);
            bChangeToMove = true;
            break;
        }
        case eTrapCastPoint:
        {
            uint trapId = m_activeMob->trapIdByPoint(dynamic_cast<CTrapCastPoint*>(pNode));
            CCreateTrapPointCommand* pUndo = new CCreateTrapPointCommand(this, trapId, !bLookPoint);
            m_pUndoStack->push(pUndo);
            bChangeToMove = true;
            break;
        }
        default:
            break;
        }

        pNode->setState(ENodeState::eDraw);

    }

    if(bChangeToMove)
        m_pOp->changeState(new CMoveAxis(this, EOperateAxisXY));
}

void CView::copySelectedIDsToClipboard()
{
    QVector<uint> arrId;
    CNode* pNode = nullptr;
    foreach(pNode, m_activeMob->nodes())
    {
        if(pNode->nodeState() != eSelect)
            continue;
        arrId.append(pNode->mapId());
    }
    std::sort(arrId.begin(), arrId.end());
    QString text;
    for(auto id : arrId)
    {
        text += QString::number(id) + "\n";
    }
    QClipboard* pClipboard = QGuiApplication::clipboard();
    pClipboard->setText(text);
}

void CView::execMobSwitch()
{
    if(nullptr != m_activeMob && m_aMob.size() == 1) //dont switch beetwen 1 mob
        return;

    CRoundMobCommand* pUndo = new CRoundMobCommand(this);
    m_pUndoStack->push(pUndo);
}

void CView::clearHistory()
{
    m_pUndoStack->clear();
}

void CView::onMobParamEditFinished(CMobParameters *pMob)
{
    m_arrParamWindow.removeOne(pMob);
}

void CView::roundActiveMob()
{
    if(m_aMob.isEmpty())
        return;

    if(nullptr == m_activeMob)
    {
        changeCurrentMob(m_aMob.front());
        return;
    }


    m_activeMob->clearSelect();
    int actIndex = m_aMob.indexOf(m_activeMob);
    if(actIndex == m_aMob.size()-1)
        actIndex = 0;
    else
        ++actIndex;

    changeCurrentMob(m_aMob[actIndex]);
}

void CView::undo_roundActiveMob()
{
    if(m_aMob.size() <1)
        return;

    if(m_aMob.size() == 1)
    {
        if(nullptr != m_activeMob)
            changeCurrentMob(nullptr);

        return;
    }

    m_activeMob->clearSelect();
    int actIndex = m_aMob.indexOf(m_activeMob);
    if(actIndex == 0)
        actIndex = m_aMob.size()-1;
    else
        --actIndex;

    changeCurrentMob(m_aMob[actIndex]);
}

void CView::execUnloadCommand()
{
    if(!m_pLand->isMprLoad())
        return;

    if(m_activeMob == nullptr)
    {
        unloadLand();
        return;
    }

    CCloseActiveMobCommand* pCommand = new CCloseActiveMobCommand(this);
    m_pUndoStack->push(pCommand);
}

void CView::iterateRoundMob()
{
    if(nullptr == m_activeMob)
        return;

    if(nullptr == m_pRoundForm)
    {
        QList<QString> arrMob;
        arrMob.append(m_activeMob->mobName());
        for(auto& mob: m_aMob)
        {
            if(mob == m_activeMob)
                continue;
            arrMob.append(mob->mobName());
        }
        m_pRoundForm = new CRoundMobForm();
        m_pRoundForm->initMobList(arrMob);
        m_pRoundForm->show();
        return;
    }
    m_pRoundForm->round();
}

void CView::applyRoundMob()
{
    if(nullptr == m_pRoundForm)
        return;

    QString newMobName = m_pRoundForm->selectedMob();
    delete m_pRoundForm;
    m_pRoundForm = nullptr;
    if (newMobName == m_activeMob->mobName())
        return;

    auto pChangeMobCommand = new CChangeActiveMobCommand(this, newMobName);
    m_pUndoStack->push(pChangeMobCommand);
}

void CView::saveSession()
{
    if(!m_pLand->isMprLoad())
        return;

    QString mprPath(m_pLand->filePath().absoluteFilePath());
    QVector<QString> arrMobPath;

    for(auto& mob: m_aMob)
    {
        if(mob == m_activeMob)
            continue;
        arrMobPath.append(mob->filePath().absoluteFilePath());
    }
    if(m_activeMob)
        arrMobPath.append(m_activeMob->filePath().absoluteFilePath());

    auto pManager = CSessionDataManager::getInstance();
    pManager->reset();
    pManager->addZoneData(mprPath, arrMobPath);
    QVector3D pos, pivot, rot;
    m_cam->getLocation(pos, pivot, rot);
    pManager->addCameraData(pos, pivot, rot);
    pManager->addQuickTileIndices(m_pTileForm->quickTileSet());
    pManager->saveSession();
}

void CView::loadSession()
{
    auto pManager = CSessionDataManager::getInstance();
    pManager->reset();
    pManager->loadSession();

    QString mprPath;
    QVector<QString> arrMobPath;
    if(!pManager->getZoneData(mprPath, arrMobPath))
        return;

    if(mprPath.isEmpty())
        return;

    QFileInfo mprFile(mprPath);
    if(!mprFile.exists())
    {
        ei::log(eLogWarning, "Cannot find MPR file: " + mprFile.absoluteFilePath());
        return;
    }
    QUndoCommand* loadMpr = new COpenCommand(this, mprFile);
    m_pUndoStack->push(loadMpr);

    for(const auto& mobPath: arrMobPath)
    {
        QFileInfo mobFile(mobPath);
        if(!mobFile.exists())
        {
            ei::log(eLogWarning, "Cannot find MOB file: " + mobFile.absoluteFilePath());
            continue;
        }
        COpenCommand* pLoadCommand = new COpenCommand(this, mobFile);
        m_pUndoStack->push(pLoadCommand);
        CRoundMobCommand* pRound = new CRoundMobCommand(this);
        m_pUndoStack->push(pRound);
    }

    QVector3D pos, pivot, rot;
    if(pManager->getCameraData(pos, pivot, rot))
    {
        m_cam->moveTo(pos);
        m_cam->setLocation(pos, pivot, rot);
    }

    QVector<int> arrQuickInd;
    if(pManager->getdQuickTileIndices(arrQuickInd))
    {
        m_pTileForm->setQuickTileSet(arrQuickInd);
    }
}

bool CView::isRecentAvailable()
{
    return m_recentOpenedFile_file.exists();
}

int CView::renameActiveMobUnits(QMap<QString, QString> &mapName)
{
    ei::log(ELogMessageType::eLogInfo, "processing map " + m_activeMob->mobName());
    int n=0;
    CUnit* pUnit = nullptr;
    QString sourceName;
    for(auto& node:m_activeMob->nodes())
    {
        if(node->nodeType() != ENodeType::eUnit)
            continue;
        pUnit = dynamic_cast<CUnit*>(node);
        sourceName = pUnit->databaseName();
        if(mapName.contains(sourceName))
        {
            QSharedPointer<IPropertyBase> newName(new propStr(eObjParam_UNIT_PROTOTYPE, mapName[sourceName]));
            pUnit->applyParam(newName);
            ++n;
            ei::log(eLogInfo, "Unit:" + QString::number(pUnit->mapId()) + " last name:" + sourceName + " new name:" + newName->toString());
        }
        else
            ei::log(ELogMessageType::eLogFatal, "not found:" + sourceName);
    }
    ei::log(ELogMessageType::eLogInfo, "processing map finished:" + m_activeMob->mobName());
    return n;
}
