#include "glwidget.h"
#include "GL/glu.h"

GLWidget::GLWidget(QWidget *parent) : QGLWidget (parent)
{
    xAxisRotation = yAxisRotation = 0;
    xViewPos = 0;
    yViewPos = 0;

    triangles = 0;
    length = 1;
    width = 1;
    height = 1;
    name = "";

    xCamPos = 0;
    yCamPos = 0;
    zCamPos = 5;

    size = 300;
    zoom = 1;
    orthosize = 6;

    open_file = false;
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

    //if (open_file == true){
        drawFigure(fig);
    //}
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
    }
    update();
}

void GLWidget::drawCube(float width, float length, float height){

    GLfloat verticesCube[] = {0,0,0,
                          width,0,0,
                          width,height,0,
                          0, height, 0,
                          0, 0, -length,
                          width, 0, -length,
                          width, height, -length,
                          0, height, -length};

    GLubyte colorsCube[] = {255,0,0,
                        155,0,0,
                        55,0,0,
                        55,55,0,
                        0,155,0,
                        0,255,255,
                        0,0,255,
                        0,0,155};

    GLubyte indices[] = {0,1,2, 2,3,0,
                        1,5,6, 6,2,1,
                        3,2,6, 6,7,3,
                        7,4,0, 0,3,7,
                        1,0,4, 4,5,1,
                        6,5,4, 4,7,6};

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glColorPointer(3, GL_UNSIGNED_BYTE, 0, colorsCube);
    glVertexPointer(3, GL_FLOAT, 0, verticesCube);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, indices);
    glDisableClientState(GL_VERTEX_ARRAY);
    triangles = sizeof (indices)/3/sizeof(GLubyte);
}

void GLWidget::drawFigure(figure fig){
    //translate on class func
    if (open_file){
        GLubyte figColors[fig.vertices.size()*3];
        //std::fill_n(figColors, fig.vertices.size()*3, 230);

        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);
        glColorPointer(3, GL_UNSIGNED_BYTE, 0, figColors);
        glVertexPointer(3, GL_FLOAT, 0, fig.vertices.data());
        glDrawElements(GL_TRIANGLES, fig.indices.size(), GL_UNSIGNED_INT, fig.indices.data());
        glDisableClientState(GL_VERTEX_ARRAY);
        triangles = fig.indices.count()/3;
    }
}
