#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QGLWidget>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QKeyEvent>

//#include <Qpoint>

#include "ei_types.h"
#include "scene.h"

class GLWidget : public QGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    explicit GLWidget (QWidget* parent = 0);
    ~GLWidget();

   void calc_select_line (float mouse_x, float mouse_y);
    //bool intersect_triangle_line (figure fig, QVector <float> p1, QVector <float> p2);
   bool setScene(CScene* scn) {return (m_scene = scn);}

private:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

    void initShaders();
    void initTextures();
private:
    QPoint m_pressPosition;

    GLfloat m_xAxisRotation;
    GLfloat m_yAxisRotation;
    GLfloat m_currentWidth;
    GLfloat m_currentHeight;
    GLfloat m_xViewPos;
    GLfloat m_yViewPos;
    QMatrix4x4 m_projection;

    float m_size;
    QVector3D m_zoom;
    f3 m_camPos;
    f3 m_volume;  // x==length y==width z==height

    CScene* m_scene;

    QOpenGLShaderProgram m_shaderPrograms;
    QOpenGLTexture* m_texture;
};

#endif // GLWIDGET_H
