#ifndef MAGICTRAP_H
#define MAGICTRAP_H
#include "objects\worldobj.h"
#include "types.h"

class CMagicTrap; //forward declaration
class CActivationZone : public CObjectBase
{
public:
    CActivationZone() = delete;
    CActivationZone(CMagicTrap* pTrap);
    CActivationZone(const CActivationZone&) = delete;
    CActivationZone(CMagicTrap* pTrap, const CActivationZone& zone);
    ~CActivationZone();
    ENodeType nodeType() override {return ENodeType::eTrapActZone; }

    uint deserialize(util::CMobParser& parser) override;
    uint serialize(util::CMobParser& parser) override;
    void deSerializeJson(QJsonObject data);
    void serializeJsonArray(QJsonArray& obj);
    //SArea getArea();
    void draw(QOpenGLShaderProgram* program) override;
    void update();

private:
    QOpenGLBuffer m_vertexBuf; //vertex buffer for drawing area activating trap
    QOpenGLBuffer m_indexBuf;
    QVector<QVector3D> m_aDrawPoint; // openGL drawing activation direction
    float m_radius;
    CMagicTrap* m_pParent;
};

class CTrapCastPoint : public CObjectBase
{
public:
    CTrapCastPoint() = delete;
    CTrapCastPoint(CMagicTrap* pTrap);
    CTrapCastPoint(const CTrapCastPoint&) = delete;
    CTrapCastPoint(CMagicTrap* pTrap, const CTrapCastPoint& pCast);
    ~CTrapCastPoint();
    ENodeType nodeType() override {return ENodeType::eTrapCastPoint; }

    uint deserialize(util::CMobParser& parser) override;
    uint serialize(util::CMobParser& parser) override;
    void deSerializeJsonArray(QJsonArray data);
    void serializeJsonArray(QJsonArray& obj);
    void draw(QOpenGLShaderProgram* program) override;

private:
    CMagicTrap* m_pParent;
};

class CMagicTrap : public CWorldObj
{
public:
    CMagicTrap();
    CMagicTrap(const CMagicTrap& trap);
    CMagicTrap(QJsonObject data);
    ~CMagicTrap();
    ENodeType nodeType() override {return ENodeType::eMagicTrap; }
    void draw(QOpenGLShaderProgram* program) override;
    uint deserialize(util::CMobParser& parser) override;
    void serializeJson(QJsonObject& obj) override;
    uint serialize(util::CMobParser& parser) override;
    void collectParams(QMap<EObjParam, QString>& aParam, ENodeType paramType) override;
    void applyParam(EObjParam param, const QString& value) override;
    QString getParam(EObjParam param) override;
    QJsonObject toJson() override;
    void loadFigure() override {}
    void loadTexture() override;

private:
    void update();

private:
    QOpenGLBuffer m_vertexBuf;
    QOpenGLBuffer m_indexBuf;
    QVector<QVector3D> m_aDrawingLine;
    int m_diplomacy; //can be -1
    QString m_spell;
    //QVector<SArea> m_aArea; // areas where trap will be activated (array of x,y,radius)
    //QVector<QVector2D> m_aTarget; //points where trap will cast spell (array of x,y)
    uint m_castInterval;
    bool m_bCastOnce;
    QVector<CActivationZone*> m_aActZone;
    QVector<CTrapCastPoint*> m_aCastPoint;



};

#endif // MAGICTRAP_H
