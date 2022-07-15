#include "preview.h"
#include "view.h"
#include "node.h"
#include "camera.h"
#include <QWheelEvent>

CPreview::CPreview(QWidget *parent):
    QGLWidget(parent)
  ,m_pNode(nullptr)
  ,m_pView(nullptr)
{
    m_cam.reset(new CCamera(QVector3D(0.0, 0.5, 5.0), QVector3D(0.0, 0.0, 0.0), -70.0f, -45.0f));
}

void CPreview::attachNode(CNode *pNode)
{
    m_pNode = pNode;
}

void CPreview::attachSettings(CSettings *pSet)
{
    m_cam->attachSettings(pSet);
}

CPreview::~CPreview()
{
    //!!! MUST DESTROY PARENT OPENGL WIDGET FIRST
    //doneCurrent();
}

void CPreview::initializeGL()
{
    makeCurrent();
    qglClearColor(Qt::darkGray);
    //glEnable(GL_EXT_texture_filter_anisotropic);
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE); //draw only front faces (disable if need draw both sides)

    glEnable(GL_ALPHA_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //doneCurrent();
}

void CPreview::resizeGL(int width, int height)
{
    makeCurrent();
    m_height = height;
    m_width = width;
    const float ratio = float(width)/height;
    glViewport(0, 0, width, height);
    m_projection.setToIdentity();
    m_projection.perspective(45, ratio, 1, 1000);
}

void CPreview::paintGL()
{
    makeCurrent();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    draw();
}



void CPreview::wheelEvent(QWheelEvent *event)
{
    event->delta() > 0 ? m_cam->strafeForward(false) : m_cam->strafeBack(false);
    updateGL();
}

void CPreview::draw()
{
    if(nullptr == m_pNode || nullptr == m_pView)
        return;


    QMatrix4x4 camMatrix = m_cam->update();

    // Bind shader pipeline for use
    QOpenGLShaderProgram* pProgram = &m_pView->shaderObject();
    if (nullptr != pProgram && !pProgram->bind())
        close();

    pProgram->setUniformValue("u_projMmatrix", m_projection);
    pProgram->setUniformValue("u_viewMmatrix", camMatrix);

    m_pNode->draw(pProgram);

}

void CPreview::refreshCam(CBox box)
{
    m_cam->moveTo(box.center());
    m_cam->moveAwayOn(box.radius()*1.5f);
    updateGL();
}

