#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QGLWidget>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QKeyEvent>
//#include <Qpoint>

#include "ei_types.h"
#include "scene.h"
#include "ei_object.h"  //!TEMP#1 for test drawing

class GLWidget : public QGLWidget
{
    Q_OBJECT
public:
    explicit GLWidget (QWidget* parent = 0);
    ~GLWidget();

    //QVector <figure> fig;
   void calc_select_line (float mouse_x, float mouse_y);
    //bool intersect_triangle_line (figure fig, QVector <float> p1, QVector <float> p2);

private:
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
    void keyPressEvent(QKeyEvent *event);
private:
    QPoint m_pressPosition;

    GLfloat m_xAxisRotation;
    GLfloat m_yAxisRotation;
    GLfloat m_currentWidth;
    GLfloat m_currentHeight;
    GLfloat m_xViewPos;
    GLfloat m_yViewPos;

    float m_size;
    float m_zoom;
    f3 m_camPos;
    f3 m_volume;  // x==length y==width z==height

    QVector<ei::CFigure*> m_figure; //!TEMP#1
};

#endif // GLWIDGET_H
