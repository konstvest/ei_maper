#ifndef NODE_H
#define NODE_H

#include "QVector"
#include <QQuaternion>
#include <QGLWidget>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include "types.h"
#include "utils.h"
#include "figure.h"
#include "color.h"

enum ENodeType
{
    eUnknown = 0
    ,eUnit
    ,eWorldObject
    ,eTorch
    ,eMagicTrap
    ,eLever
    ,eLight
    ,eSound
    ,eParticle
};

//enum EObjParam
//{
//    eNID = 1
//    ,eName = 1<<1
//    ,eModelName = 1<<2
//todo:
//};

//TODO: params as template



class CNode
{
public:
    CNode();
    CNode(CNode* parent);
    static int s_freeId;
    virtual ~CNode();
    virtual void draw(QOpenGLShaderProgram* program = nullptr) { Q_UNUSED(program);}
    virtual void drawSelect(QOpenGLShaderProgram* program = nullptr) {Q_UNUSED(program);}
    virtual ENodeType nodeType() = 0;
    virtual uint deserialize(util::CMobParser& parser) {Q_UNUSED(parser); Q_ASSERT("pure virtual" && false); return 0;}
    virtual QString& modelName() = 0;
    virtual QString textureName() = 0;
    virtual QVector3D& minPosition() = 0;
    virtual void updateFigure(ei::CFigure* fig) = 0;
    virtual void updateVisibleParts() {}
    virtual void setTexture(QOpenGLTexture* texture) {Q_UNUSED(texture);}

    void addChild(CNode* child) {m_aChild.append(child); child->setParent(this); }
    void setParent(CNode* parent) {m_parent = parent;}
    int id() {return m_id; }
    uchar* color() {return m_pickingColor.rgb; }
    bool isColorSuitable(SColor& color) {return m_pickingColor == color; }
    void setPos(float x, float y, float z) {m_position = QVector3D(x, y, z);}
    void setPos(QVector3D& pos) {m_position = pos;}
    void setXPos(float x) {m_position.setX(x);}
    void setYPos(float y) {m_position.setY(y);}
    void setZPos(float z) {m_position.setZ(z);}
    void setRot(QVector4D& quat);
    void setXRot(float xAngle) {m_rotation.x = xAngle; applyRotation();}
    void setYRot(float yAngle) {m_rotation.y = yAngle; applyRotation();}
    void setZRot(float zAngle) {m_rotation.z = zAngle; applyRotation();}
    void rotate(QQuaternion& quat);
    void move(float x, float y, float z);
    QVector3D& position() {return m_position; }
    void setDrawPosition(QVector3D pos) {m_drawPosition = pos;}

private:
    void applyRotation();
    void init();

protected:
    QVector3D m_position; // offset from landscape
    QVector3D m_drawPosition; // absolute position for object
    F3 m_rotation;
    uint m_mapID;
    QString m_name;
    QString m_comment;
    QMatrix4x4 m_rotateMatrix;
    QVector<CNode*> m_aChild;
    CNode* m_parent;
    SColor m_pickingColor;

private:
    int m_id;

};

#endif // NODE_H
