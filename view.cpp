#include <QtWidgets>
#include <QDebug>
#include <QtMath>       //for fabs

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

class CLogic;

CView::CView(QWidget* parent):
    QGLWidget (parent)
    , m_landscape(nullptr)
    , m_objList(nullptr)
    , m_textureList(nullptr)
    , m_logger(nullptr)
    , m_pSettings(nullptr)
{
    setFocusPolicy(Qt::ClickFocus);

    m_cam.reset(new CCamera);
    m_timer = new QTimer;
    m_keyManager.reset(new CKeyManager);
    m_aReadState.resize(eReadCount);

    m_cam->attachKeyManage(m_keyManager.get());
    connect(m_timer, SIGNAL(timeout()), this, SLOT(updateWindow()));
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
    if(nullptr == m_textureList)
        m_textureList.reset(new CTextureList);

    return m_textureList.get();
}
CObjectList* CView::objList()
{
    if(nullptr == m_objList)
        m_objList.reset(new CObjectList);

    return m_objList.get();
}

void CView::attachLogWindow(QTextEdit* pTextEdit)
{
    m_logger.reset(new CLogger(pTextEdit));
}

void CView::attachSettings(CSettings* pSettings)
{
    m_pSettings = pSettings;
}

void CView::updateWindow()
{
    updateGL();
}

//void CView::setXRot(int angle)
//{
//    util::qNormalizeAngle(angle);
//    for (auto& node: m_aNodeSelected)
//        node->setXRot(float(angle));
//}

//void CView::setYRot(int angle)
//{
//    util::qNormalizeAngle(angle);
//    for (auto& node: m_aNodeSelected)
//        node->setYRot(float(angle));
//}

//void CView::setZRot(int angle)
//{
//    util::qNormalizeAngle(angle);
//    for (auto& node: m_aNodeSelected)
//        node->setZRot(float(angle));
//}

//void CView::setXOffset(int offset)
//{
//    for (auto& node: m_aNodeSelected)
//        node->setXPos(float(offset)/50);
//}

//void CView::setYOffset(int offset)
//{
//    for (auto& node: m_aNodeSelected)
//        node->setYPos(float(offset)/50);
//}

//void CView::setZOffset(int offset)
//{
//    for (auto& node: m_aNodeSelected)
//        node->setZPos(float(offset)/50);
//}

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
    glEnable(GL_CULL_FACE); //draw only front faces (disable if need draw both sides)

    glEnable(GL_ALPHA_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    initShaders();
    //loadResource();
}

void CView::resizeGL(int width, int height)
{
    m_height = height;
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
    m_landProgram.setUniformValue("u_watcherPos", m_cam->pos());
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

    m_program.setUniformValue("u_highlight", false);

    for(auto& mob: m_aMob)
        for (auto node: mob->nodes())
            node->draw(&m_program);

    m_program.setUniformValue("u_highlight", true);
    for(auto& mob: m_aMob)
        for (auto& node: mob->nodesSelected())
            node->draw(&m_program);


}

//void CView::rotate(QQuaternion& quat)
//{
//    for (auto& node: m_aNodeSelected)
//        node->rotate(quat);
//    updateGL();
//}

bool CView::isResourceInitiated()
{
    bool res = true;
    COptString* opt = dynamic_cast<COptString*>(m_pSettings->opt(eOptSetResource, "figPath1"));
    if(opt->value().isEmpty())
    {
        QMessageBox::warning(this, "Warning","Choose path to figures.res");
        m_pSettings->onShow(eOptSetResource);
        res = false;
    }
    else
    {
        objList()->addResourceFile(opt->value());
        opt = dynamic_cast<COptString*>(m_pSettings->opt(eOptSetResource, "figPath2"));
        if(!opt->value().isEmpty())
            objList()->addResourceFile(opt->value());
    }

    if (res)
    {
        opt = dynamic_cast<COptString*>(m_pSettings->opt(eOptSetResource, "texPath1"));
        if(opt->value().isEmpty())
        {
            QMessageBox::warning(this, "Warning","Choose path to textures.res");
            m_pSettings->onShow(eOptSetResource);
            res = false;
        }
        else
        {
            texList()->addResourceFile(opt->value());
            opt = dynamic_cast<COptString*>(m_pSettings->opt(eOptSetResource, "texPath2"));
            if(!opt->value().isEmpty())
                texList()->addResourceFile(opt->value());
        }
    }

    return res;
}

void CView::loadLandscape(QFileInfo& filePath)
{
    if(!isResourceInitiated())
        return;

    log("Loading landscape");
    m_landscape = new CLandscape;
    m_landscape->setParentView(this);
    m_landscape->readMap(filePath);
    log("Landscape loaded");
    m_timer->setInterval(15);
    m_timer->start();
}

void CView::unloadLand()
{
    if(m_landscape)
        delete m_landscape;

    m_landscape = nullptr;
    log("landscape unloaded");
}

void CView::log(const char* msg)
{
    emit updateMsg(msg);
    //m_logger->log(msg);
}

void CView::loadMob(QFileInfo &filePath)
{
    Q_ASSERT(m_objList); // for correctly update figures
    Q_ASSERT(m_textureList); // for correctly update textures
    CMob* mob = new CMob;
    mob->attachView(this);
    mob->readMob(filePath);
    m_aMob.append(mob);

    // update position on the map for each node
    Q_ASSERT(m_landscape);
    m_landscape->projectPositions(mob->nodes());
}

void CView::saveMob(QFileInfo& file)
{
//    for(auto& mob: m_aMob)
//    {
//        mob->serializeJson(file);
//    }
}

void CView::serializeMob(QFileInfo &file)
{


    for(auto& mob: m_aMob)
    {
        mob->serializeJson(file);
    }
}

void CView::unloadMob()
{
    for(auto mob: m_aMob)
        delete mob;

    m_aMob.clear();

}

// Deletes selected nodes from mob
void CView::delNodes()
{
    for(auto& mob: m_aMob)
        mob->delNodes();
}
