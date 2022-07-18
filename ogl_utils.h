#ifndef CSELECTFRAME_H
#define CSELECTFRAME_H

#include <QVector>
#include <QVector2D>
#include <QRect>
#include <QOpenGLBuffer>
#include <QMatrix4x4>

//forward declarations
class QOpenGLShaderProgram;


class CSelectFrame
{
public:
    CSelectFrame();
    ~CSelectFrame();
    void draw(QOpenGLShaderProgram* program);
    void updateFrame(QPointF& bottomLeft, QPointF& topRight);
    void reset();

private:
    QOpenGLBuffer m_vertexBuf; //vertex buffer for drawing links between look point and patrol point
    QOpenGLBuffer m_indexBuf;
    QMatrix4x4 m_matrix;
    QVector<QVector3D> m_aPoints; //use QVector3D?
};

#endif // CSELECTFRAME_H
