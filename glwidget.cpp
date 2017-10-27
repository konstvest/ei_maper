#include "glwidget.h"
#include "GL/glu.h"

GLWidget::GLWidget(QWidget *parent) : QGLWidget (parent)
{
    xAxisRotation = yAxisRotation = 0;
    xViewPos = 0;
    yViewPos = 0;

    //triangles = 0;
    length = 1;
    width = 1;
    height = 1;
    name = "";

    xCamPos = 0;
    yCamPos = 0;
    zCamPos = 5;

    zoom = 1;

}

GLWidget::~GLWidget(){
    qDebug() << "GLWidget destructor :(";
}

void GLWidget::initializeGL(){
    glClearColor(0.2, 0.2, 0.2, 1);
}

void GLWidget::resizeGL(int w, int h){
    currentWidth = w;
    currentHeight = h;
    if (w<h)
        size = w;
    else
        size = h;
}

void GLWidget::paintGL(){

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(0, 0, currentWidth, currentHeight);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, (GLfloat)currentWidth/currentHeight, 0.1f, 100.0f);
    gluLookAt(xCamPos, yCamPos, zCamPos, 0, 0, 0, 0, 1, 0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);   //render depend on normals (use in game)
    glRotatef(xAxisRotation, 0.0, 1.0, 0.0);
    glRotatef(yAxisRotation, 1.0, 0.0, 0.0);
    glScalef(zoom, zoom, zoom);

    drawFigure(fig);
}

void GLWidget::mousePressEvent(QMouseEvent *event){
    pressPosition = event->pos();
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
        xAxisRotation += 2*((GLfloat)event->x()-(GLfloat)pressPosition.x());
        yAxisRotation += 2*((GLfloat)event->y()-(GLfloat)pressPosition.y());
        pressPosition = event->pos();
    }
    update();
}

void GLWidget::wheelEvent(QWheelEvent *event){
    zoom += (float)event->delta()/500;
    update();
}

void GLWidget::keyPressEvent(QKeyEvent *event){
    switch (event->key()){
    case Qt::Key_W:
        zCamPos -=0.35;
        break;
    case Qt::Key_S:
        zCamPos +=0.35;
        break;
    case Qt::Key_A:
        xCamPos -= 0.4;
        break;
    case Qt::Key_D:
        xCamPos += 0.4;
        break;
    case Qt::Key_Q:
        yCamPos +=0.2;
        break;
    case Qt::Key_E:
        yCamPos -=0.2;
        break;
    case Qt::Key_Up:
        size +=10;
        break;
    case Qt::Key_Down:
        size -=10;
        break;
    case Qt::Key_C:

        break;
    }
    update();
}

void GLWidget::drawFigure(QVector <figure> fig){
    //translate on class func
    triangles = 0;
    for (int i(0); i<fig.size(); i++){
        GLubyte figColors[fig[i].vertices.size()*3];
        //std::fill_n(figColors, fig.vertices.size()*3, 230);

        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);
        glColorPointer(3, GL_UNSIGNED_BYTE, 0, figColors);
        glVertexPointer(3, GL_FLOAT, 0, fig[i].vertices.data());
        glDrawElements(GL_TRIANGLES, fig[i].indices.size(), GL_UNSIGNED_INT, fig[i].indices.data());
        glDisableClientState(GL_VERTEX_ARRAY);
        triangles += fig[i].indices.count()/3;
    }
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
    vy = currentHeight-mouse_y;
    glReadPixels(vx, vy, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &vz);
    //vz = -1;

    gluUnProject(vx, vy, vz, modelView, projection, vport, &nearPoint[0], &nearPoint[1], &nearPoint[2]);
    vz = 1;
    gluUnProject(wx, wy, wz, modelView, projection, vport, &farPoint[0], &farPoint[1], &farPoint[2]);
    qDebug() << "near x:" << *(nearPoint) << " y:" << *(nearPoint+1) << " z:" << *(nearPoint+2) << " far x:" << *(farPoint) << " y:" << *(farPoint+1) << " z:" << *(farPoint+2);

}

//bool GLWidget::intersect_triangle_line(figure fig, QVector<float> p1, QVector<float> p2){

//}
