#include "object_base.h"

CObjectBase::CObjectBase():
    m_texture(nullptr)
{
}

CObjectBase::CObjectBase(CNode* node):
    CNode(node)
    ,m_texture(nullptr)
{
}

CObjectBase::~CObjectBase()
{
    for(auto& part: m_aPart)
        delete part;
}

void CObjectBase::updateFigure(ei::CFigure* fig)
{
    Q_ASSERT(fig);
    fig->getVertexData(m_aPart, m_complection);

    float min(1000.0f); // 0.0f?
    for(auto& part: m_aPart)
        for(auto& vert : part->vertData())
            if (vert.position.z() < min)
                min = vert.position.z();

    m_minPoint = QVector3D(0.0f, 0.0f, min);
}

void CObjectBase::updateVisibility(QVector<QString>& aPart)
{
    if (aPart.isEmpty())
        for(auto& part: m_aPart)
            part->setVisible(true);
    else
    {
        for(auto& part : m_aPart)
            part->setVisible(aPart.indexOf(part->name()) != -1);
    }
}

void CObjectBase::setTexture(QOpenGLTexture* texture)
{
    m_texture = texture;
}

void CObjectBase::draw(QOpenGLShaderProgram* program)
{
    Q_ASSERT(m_texture);
    if(m_texture == nullptr)
        return;
    m_texture->bind(0);
    if (!m_parent)
    {
        QMatrix4x4 matrix;
        matrix.setToIdentity();
        matrix.translate(m_drawPosition);
        matrix = matrix * m_rotateMatrix;
        program->setUniformValue("u_modelMmatrix", matrix);
    }

    program->setUniformValue("qt_Texture0", 0);

    for(auto& part: m_aPart)
        part->draw(program);
}

void CObjectBase::drawSelect(QOpenGLShaderProgram* program)
{
    Q_ASSERT(m_texture);
    m_texture->bind(0);
    if(m_texture == nullptr)
        return;

    if (!m_parent)
    {
        QMatrix4x4 matrix;
        matrix.setToIdentity();
        matrix.translate(m_drawPosition);
        matrix = matrix * m_rotateMatrix;
        program->setUniformValue("u_modelMmatrix", matrix);
        program->setUniformValue("u_color", m_pickingColor.toVec4());
    }

    for(auto& part: m_aPart)
        part->drawSelect(program);
}


