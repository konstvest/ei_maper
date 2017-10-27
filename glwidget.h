#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QGLWidget>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QKeyEvent>
#include <Qpoint>

#include "ei_types.h"
#include "scene.h"

class GLWidget : public QGLWidget
{
    Q_OBJECT
public:
    explicit GLWidget (QWidget* parent = 0);
    ~GLWidget();
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

    QVector <figure> fig;

    void calc_select_line (float mouse_x, float mouse_y);
    bool intersect_triangle_line (figure fig, QVector <float> p1, QVector <float> p2);

private:
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void drawFigure(QVector <figure> fig);
};

#endif // GLWIDGET_H
