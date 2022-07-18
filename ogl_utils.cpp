#include "ogl_utils.h"

#include <QOpenGLShaderProgram>

CSelectFrame::CSelectFrame():
    m_indexBuf(QOpenGLBuffer::IndexBuffer)
{
    m_matrix.setToIdentity();
}

CSelectFrame::~CSelectFrame()
{
    m_vertexBuf.destroy();
    m_indexBuf.destroy();
}

void CSelectFrame::draw(QOpenGLShaderProgram *program)
{
    if(m_aPoints.size() == 0) // do not draw looking point if absent
        return;

    glDisable(GL_DEPTH_TEST);
    //matrix.setToIdentity();
    program->setUniformValue("u_modelMmatrix", m_matrix);

    program->setUniformValue("u_bUseColor", true);
    program->setUniformValue("customColor", QVector4D(0.05, 0.78, 0.93, 0.3));
    int offset(0);
    // Tell OpenGL which VBOs to use

    m_vertexBuf.bind();
    // Tell OpenGL programmable pipeline how to locate vertex position data
    int vertexLocation = program->attributeLocation("a_position");
    program->enableAttributeArray(vertexLocation);
    program->setAttributeBuffer(vertexLocation, GL_FLOAT, offset, 3, int(sizeof(QVector3D)));

//    offset+=int(sizeof(QVector3D)); // size of vertex position
//    int normLocation = program->attributeLocation("a_normal");
//    program->enableAttributeArray(normLocation);
//    program->setAttributeBuffer(normLocation, GL_FLOAT, offset, 3, int(sizeof(SVertexData)));

//    offset+=int(sizeof(QVector3D)); // size of normal
//    int textureLocation = program->attributeLocation("a_texture");
//    program->enableAttributeArray(textureLocation);
//    program->setAttributeBuffer(textureLocation, GL_FLOAT, offset, 2, int(sizeof(SVertexData)));

    // Draw cube geometry using indices from VBO 1
    m_indexBuf.bind();
    glDrawElements(GL_TRIANGLES, m_indexBuf.size(), GL_UNSIGNED_SHORT, nullptr);
    program->setUniformValue("customColor", QVector4D(0.0, 0.0, 0.0, 0.0));
    glEnable(GL_DEPTH_TEST);
}

void CSelectFrame::updateFrame(QPointF& bottomLeft, QPointF& topRight)
{
    m_aPoints.clear();
//    m_aPoints.append(QVector3D(rect.bottomLeft().x(), rect.bottomLeft().y(), 0.0f));
//    m_aPoints.append(QVector3D(rect.topLeft().x(), rect.topLeft().y(), 0.0f));
//    m_aPoints.append(QVector3D(rect.topRight().x(), rect.topRight().y(), 0.0f));
//    m_aPoints.append(QVector3D(rect.bottomRight().x(), rect.bottomRight().y(), 0.0f));
    m_aPoints.append(QVector3D(bottomLeft.x(), bottomLeft.y(), 0.0f));
    m_aPoints.append(QVector3D(bottomLeft.x(), topRight.y(), 0.0f));
    m_aPoints.append(QVector3D(topRight.x(), topRight.y(), 0.0f));
    m_aPoints.append(QVector3D(topRight.x(), bottomLeft.y(), 0.0f));

    // Generate VBOs and transfer data
    m_vertexBuf.create();
    m_vertexBuf.bind();
    m_vertexBuf.allocate(m_aPoints.data(), m_aPoints.count() * int(sizeof(QVector3D)));
    m_vertexBuf.release();

    QVector<ushort> aInd;
    aInd.append(0);
    aInd.append(1);
    aInd.append(3);

    aInd.append(1);
    aInd.append(2);
    aInd.append(3);

    m_indexBuf.create();
    m_indexBuf.bind();
    m_indexBuf.allocate(aInd.data(), aInd.count() * int(sizeof(ushort)));
    m_indexBuf.release();
}

void CSelectFrame::reset()
{
    m_aPoints.clear();
}
