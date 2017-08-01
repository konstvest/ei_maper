#ifndef MAINSCENE_H
#define MAINSCENE_H

#include <QTimer>
#include <QGLWidget>
#include <QPoint>
#include <QDebug>
#include <QMouseEvent>
#include <QOpenGLFunctions>

class MainScene : public QGLWidget
{
    Q_OBJECT

private:
    GLfloat cubeVertexArray[8][3];
    GLfloat cubeColorArray[8][3];
    GLubyte cubeIndexArray[6][4];
    GLfloat cubeTextureArray[8][2];
    QPoint pressPosition;
    QPoint releasePosition;
    GLfloat xAxisRotation;
    GLfloat yAxisRotation;
    GLfloat currentWidth;
    GLfloat currentHeight;
    GLuint textures[6];

    void generateTextures();

private slots:

protected:
    void initializeGL();
    void resizeGL(int nWidth, int nHeight);
    void paintGL();
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);

public:
    MainScene(QWidget *parent = 0);
    ~MainScene();

};

#endif // MAINSCENE_H
