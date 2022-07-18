#include "part.h"

CPart::CPart():
   m_indexBuf(QOpenGLBuffer::IndexBuffer)
  ,m_bShow(true)
{
    m_aVertData.clear();
}

CPart::CPart(const CPart &part):
    m_indexBuf(QOpenGLBuffer::IndexBuffer)
{
    m_name = part.m_name;
    m_bShow = part.m_bShow;
    m_aVertData = part.m_aVertData;
    update(); //create new index and vertex buffers
}

CPart::~CPart()
{
    m_vertexBuf.destroy();
    m_indexBuf.destroy();
}

void CPart::update()
{
    // Generate VBOs and transfer data
    m_vertexBuf.create();
    m_vertexBuf.bind();
    m_vertexBuf.allocate(m_aVertData.data(), m_aVertData.count() * int(sizeof(SVertexData)));
    m_vertexBuf.release();

    QVector<ushort> aInd;
    for (ushort i(0); i<m_aVertData.size(); ++i)
        aInd.append(i);

    m_indexBuf.create();
    m_indexBuf.bind();
    m_indexBuf.allocate(aInd.data(), aInd.count() * int(sizeof(ushort)));
    m_indexBuf.release();
}

void CPart::draw(QOpenGLShaderProgram* program)
{
    if (!m_bShow || m_aVertData.count() == 0)
        return;

    int offset(0);
    // Tell OpenGL which VBOs to use
    m_vertexBuf.bind();
    // Tell OpenGL programmable pipeline how to locate vertex position data
    int vertexLocation = program->attributeLocation("a_position");
    program->enableAttributeArray(vertexLocation);
    program->setAttributeBuffer(vertexLocation, GL_FLOAT, offset, 3, int(sizeof(SVertexData)));

    offset+=int(sizeof(QVector3D)); // size of vertex position
    int normLocation = program->attributeLocation("a_normal");
    program->enableAttributeArray(normLocation);
    program->setAttributeBuffer(normLocation, GL_FLOAT, offset, 3, int(sizeof(SVertexData)));

    offset+=int(sizeof(QVector3D)); // size of normal
    int textureLocation = program->attributeLocation("a_texture");
    program->enableAttributeArray(textureLocation);
    program->setAttributeBuffer(textureLocation, GL_FLOAT, offset, 2, int(sizeof(SVertexData)));

    // Draw cube geometry using indices from VBO 1
    m_indexBuf.bind();
    glDrawElements(GL_TRIANGLES, m_aVertData.count(), GL_UNSIGNED_SHORT, nullptr);
}


void CPart::drawSelect(QOpenGLShaderProgram *program)
{
    if (!m_bShow || m_aVertData.count() == 0)
        return;

    int offset(0);
    // Tell OpenGL which VBOs to use
    m_vertexBuf.bind();
    // Tell OpenGL programmable pipeline how to locate vertex position data
    int vertexLocation = program->attributeLocation("a_position");
    program->enableAttributeArray(vertexLocation);
    program->setAttributeBuffer(vertexLocation, GL_FLOAT, offset, 3, int(sizeof(SVertexData)));

    // Draw cube geometry using indices from VBO 1
    m_indexBuf.bind();
    glDrawElements(GL_TRIANGLES, m_aVertData.count(), GL_UNSIGNED_SHORT, nullptr);
}
