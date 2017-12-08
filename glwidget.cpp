#include "glwidget.h"
#include "GL/glu.h"
#include "scene.h"

GLWidget::GLWidget(QWidget *parent) :
    QGLWidget (parent),
    m_xAxisRotation (0),
    m_yAxisRotation (0),
    m_xViewPos (0),
    m_yViewPos (0),
    m_zoom (1.0, 1.0, 1.0),
    //m_camPos (0.0, 0.0, 5.0),
    m_camPos (0.0, 0.0, -7.0),
    m_volume (1.0, 1.0, 1.0),
    m_scene(0),
    m_shaderPrograms(0)
{
    //QString path("c:\\Users\\konstantin.bezelians\\files\\temp\\hd.fig");
    //m_figure.loadFromFile((wchar_t*)QString(path).utf16());
}

GLWidget::~GLWidget(){
//    makeCurrent();
//    doneCurrent();
    qDebug() << "GLWidget destructor :(";
}

void GLWidget::initializeGL(){
    initializeOpenGLFunctions();
    glClearColor(0.2, 0.2, 0.2, 1);
    initShaders();
    initTextures();
    glEnable(GL_DEPTH_TEST);
}

void GLWidget::resizeGL(int w, int h){
//    m_currentWidth = w;
//    m_currentHeight = h;
//    m_size = w < h ? w : h;
    qreal aspect = qreal(w) / qreal(h ? h : 1);
    const qreal zNear (0.5), zFar (10.0), fov (45.0);
    m_projection.setToIdentity();
    m_projection.perspective(fov, aspect, zNear, zFar);
}

void GLWidget::paintGL(){

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    m_texture->bind();;
//    glViewport(0, 0, m_currentWidth, m_currentHeight);
//    glMatrixMode(GL_PROJECTION);
//    glLoadIdentity();
//    gluPerspective(45.0f, (GLfloat)m_currentWidth/m_currentHeight, 0.1f, 100.0f);
//    gluLookAt(m_camPos.x, m_camPos.y, m_camPos.z, 0, 0, 0, 0, 1, 0);

//    glMatrixMode(GL_MODELVIEW);
//    glLoadIdentity();

//    //glEnable(GL_CULL_FACE);   //render depend on normals (use in game)
//    glRotatef(m_xAxisRotation, 0.0, 1.0, 0.0);
//    glRotatef(m_yAxisRotation, 1.0, 0.0, 0.0);
//    glScalef(m_zoom, m_zoom, m_zoom);

    //todo translate matrix and texture to shader

    QMatrix4x4 matrix;
    //matrix.translate(0.0, -1.0, -7.0);
    matrix.scale(m_zoom);
    matrix.translate(m_camPos.x, m_camPos.y, m_camPos.z);

    m_shaderPrograms.setUniformValue("mvp_matrix", m_projection * matrix);
    m_shaderPrograms.setUniformValue("texture", 0);
    m_scene->draw(&m_shaderPrograms);
}

void GLWidget::mousePressEvent(QMouseEvent *event){
    m_pressPosition = event->pos();
    setFocus();
    update();
}

void GLWidget::mouseMoveEvent(QMouseEvent *event){
    if (event->buttons() & Qt::LeftButton){
        /*xViewPos +=(GLfloat)event->x() - (GLfloat)pressPosition.x();
        yViewPos -=(GLfloat)event->y() - (GLfloat)pressPosition.y();
        pressPosition = event->pos();*/
        calc_select_line((float)event->x(), (float)event->y());
    } else if (event->buttons() & Qt::RightButton){
        m_xAxisRotation += 2*((GLfloat)event->x()-(GLfloat)m_pressPosition.x());
        m_yAxisRotation += 2*((GLfloat)event->y()-(GLfloat)m_pressPosition.y());
        m_pressPosition = event->pos();
    }
    update();
}

void GLWidget::wheelEvent(QWheelEvent *event){
    float dlt = float(event->delta())/500;
    m_zoom += QVector3D(dlt, dlt, dlt);
    update();
}

void GLWidget::keyPressEvent(QKeyEvent *event){
    switch (event->key()){
    case Qt::Key_D:{
        m_camPos.x += 0.4;
    }
        break;
    case Qt::Key_A:{
        m_camPos.x -= 0.4;
    }
        break;
    case Qt::Key_Q:{
        m_camPos.y +=0.2;
    }
        break;
    case Qt::Key_E:{
        m_camPos.y -=0.2;
    }
        break;
    case Qt::Key_S:{
        m_camPos.z +=0.35;
    }
        break;
    case Qt::Key_W:{
        m_camPos.z -=0.35;
    }
        break;
    case Qt::Key_Up:{
        m_size +=10;
    }
        break;
    case Qt::Key_Down:{
        m_size -=10;
    }
        break;
    case Qt::Key_C:

        break;
    }
    update();
}

void GLWidget::calc_select_line(float mouse_x, float mouse_y){
    GLint vport[4];
    GLdouble projection[16];
    GLdouble modelView[16];
    double vx;
    double vy;
    double vz;
    float wx, wy, wz;
    GLdouble nearPoint[3];
    GLdouble farPoint[3];

    glGetIntegerv(GL_VIEWPORT, vport);
    glGetDoublev(GL_PROJECTION_MATRIX, projection);
    glGetDoublev(GL_MODELVIEW_MATRIX, modelView);

    vx = mouse_x;
    vy = m_currentHeight-mouse_y;
    glReadPixels(vx, vy, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &vz);
    //vz = -1;

    gluUnProject(vx, vy, vz, modelView, projection, vport, &nearPoint[0], &nearPoint[1], &nearPoint[2]);
    vz = 1;
    gluUnProject(wx, wy, wz, modelView, projection, vport, &farPoint[0], &farPoint[1], &farPoint[2]);
    qDebug() << "near x:" << *(nearPoint) << " y:" << *(nearPoint+1) << " z:" << *(nearPoint+2) << " far x:" << *(farPoint) << " y:" << *(farPoint+1) << " z:" << *(farPoint+2);

}

void GLWidget::initShaders(){
    // Compile vertex shader
    if (!m_shaderPrograms.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/vshader.glsl"))
        close();

    // Compile fragment shader
    if (!m_shaderPrograms.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/fshader.glsl"))
        close();

    // Link shader pipeline
    if (!m_shaderPrograms.link())
        close();

    // Bind shader pipeline for use
    if (!m_shaderPrograms.bind())
        close();

}

void GLWidget::initTextures(){
    m_texture = new QOpenGLTexture(QImage(":/flipped_tipy.jpg").mirrored());
    m_texture->setMinificationFilter(QOpenGLTexture::Nearest);
    m_texture->setMagnificationFilter(QOpenGLTexture::Linear);
    m_texture->setWrapMode(QOpenGLTexture::Repeat);
}

//bool GLWidget::intersect_triangle_line(figure fig, QVector<float> p1, QVector<float> p2){

//}
