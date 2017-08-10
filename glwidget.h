#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QGLWidget>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QKeyEvent>
#include <Qpoint>
#include <QDebug>
#include <QString>

class GLWidget : public QGLWidget
{
    Q_OBJECT
public:
    explicit GLWidget (QWidget* parent = 0);
    int triangles;

    float orthosize;
    float size;
    float zoom;
    float xCamPos;
    float yCamPos;
    float zCamPos;
    float length;
    float width;
    float height;

    GLfloat xAxisRotation;
    GLfloat yAxisRotation;
    GLfloat currentWidth;
    GLfloat currentHeight;
    GLfloat xViewPos;
    GLfloat yViewPos;

    QString name;
    QPoint pressPosition;

protected:
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void drawCube (float width, float length, float height);
};

#endif // GLWIDGET_H
