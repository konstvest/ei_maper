#ifndef NODE_H
#define NODE_H

#include "QVector"
#include <QQuaternion>
#include <QGLWidget>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QJsonObject>

#include "types.h"
#include "utils.h"
#include "figure.h"

enum ENodeType
{
    eUnknown = 0x0
    ,eBaseType        =0x1    //0000 0001
    ,eWorldObject   =0x3    //0000 0011
    ,eUnit          =0x7    //0000 0111
    ,eTorch         =0xB    //0000 1011
    ,eMagicTrap     =0x13   //0001 0011
    ,eLever         =0x23   //0010 0011
    ,eLight         =0x5    //0000 0101
    ,eSound         =0x9    //0000 1001
    ,eParticle      =0x11   //0001 0001

    ,ePatrolPoint   =0x31   //0011 0001
    ,eLookPoint     =0x51   //0101 0001

    ,eTrapActZone   =0x91 //1001 0001
    ,eTrapCastPoint =0xC1 //1100 0001
};

enum ENodeState
{
    eDraw = 1
    ,eSelect
    ,eHidden
};

class CMob;

class CNode : public QObject
{
    Q_OBJECT
public:
    CNode();
    CNode(CNode* parent);
    CNode(const CNode& node);
    static uint s_freeId;
    virtual ~CNode();
    virtual void draw(bool isActive, QOpenGLShaderProgram* program = nullptr) = 0;
    virtual void drawSelect(QOpenGLShaderProgram* program = nullptr) = 0;
    virtual ENodeType nodeType() = 0;
    virtual uint deserialize(util::CMobParser& parser) {Q_UNUSED(parser); Q_ASSERT("pure virtual" && false); return 0;} //todo: define this method in CObjectBase
    virtual QString& modelName() = 0;
    virtual QString textureName() = 0;
    virtual QVector3D& minPosition() = 0;
    virtual void updateFigure(ei::CFigure* fig) = 0;
    virtual void loadFigure() = 0;
    virtual void setTexture(QOpenGLTexture* texture) = 0;
    virtual void loadTexture() = 0;
    virtual void serializeJson(QJsonObject& obj) = 0;
    virtual uint serialize(util::CMobParser& parser) = 0;
    virtual void collectParams(QMap<EObjParam, QString>& aParam, ENodeType paramType) = 0;
    virtual void collectlogicParams(QMap<EObjParam, QString>& aParam, ENodeType paramType) = 0;
    virtual void applyParam(EObjParam param, const QString& value) = 0;
    virtual QString getParam(EObjParam param) = 0;
    virtual QString getLogicParam(EObjParam param) = 0;
    virtual void applyLogicParam(EObjParam param, const QString& value) = 0;
    virtual bool updatePos(QVector3D& pos) = 0;
    virtual const QVector3D& complection() = 0;
    virtual void setRot(const QQuaternion& quat);
    virtual void setConstitution(QVector3D& vec) = 0;
    virtual const QVector3D& constitution() = 0;
    virtual QJsonObject toJson() = 0;
    virtual CBox getBBox() = 0;
    virtual void markAsDeleted(bool bDeleted = true) = 0;
    virtual bool isMarkDeleted() = 0;

    const uint& innerId() {return m_id; }
    const uint& mapId(){return m_mapID;}
    void setMapId(uint id) {m_mapID = id;}
    const QString& prototypeName(){return m_name;}
    void addChild(CNode* child) {m_aChild.append(child); child->setParent(this); }
    void setParent(CNode* parent) {m_parent = parent;}
    uchar* color() {return m_pickingColor.rgb; }
    bool isColorSuitable(const SColor& color) {return m_pickingColor == color; }
    void setPos(QVector3D& pos) {m_position = pos;}
    void setRot(const QVector4D& quat);
    QVector3D getEulerRotation();
    void rotate(QQuaternion& quat);
    void move(float x, float y, float z);
    QVector3D& position() {return m_position; }
    QVector3D& drawPosition() {return m_drawPosition;}
    void setDrawPosition(QVector3D pos) {m_drawPosition = pos;}
    void setState(ENodeState state) {m_state = state;}
    ENodeState nodeState() {return m_state;}

private:
    void applyRotation();
    void init();

protected:
    QVector3D m_position; // offset from landscape
    QVector3D m_drawPosition; // absolute position for object
    //QVector3D m_rotation;
    uint m_mapID;
    QString m_name;
    QString m_comment;
    QMatrix4x4 m_rotateMatrix;
    QVector<CNode*> m_aChild;
    CNode* m_parent;
    SColor m_pickingColor;
    ENodeState m_state;

private:
    uint m_id;

};

#endif // NODE_H
