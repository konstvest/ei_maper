#ifndef MAGICTRAP_H
#define MAGICTRAP_H
#include "objects\worldobj.h"
#include "types.h"

class CMagicTrap; //forward declaration
class CActivationZone : public CObjectBase
{
    Q_OBJECT
public:
    CActivationZone() = delete;
    CActivationZone(CMagicTrap* pTrap);
    CActivationZone(const CActivationZone&) = delete;
    CActivationZone(CMagicTrap* pTrap, const CActivationZone& zone);
    ~CActivationZone();
    ENodeType nodeType() override {return ENodeType::eTrapActZone; }

    void collectlogicParams(QList<QSharedPointer<IPropertyBase>>& aProp, ENodeType paramType) override;
    void getLogicParam(QSharedPointer<IPropertyBase>& prop, EObjParam propType) override;
    void applyLogicParam(const QSharedPointer<IPropertyBase>& prop) override;
    uint deserialize(util::CMobParser& parser) override;
    uint serialize(util::CMobParser& parser) override;
    void deSerializeJson(QJsonObject data);
    void serializeJsonArray(QJsonArray& obj);
    //SArea getArea();
    void draw(bool isActive, QOpenGLShaderProgram* program) override;
    void update();
    bool updatePos(QVector3D& pos) override;
    void markAsDeleted(bool bDeleted = true) override;

signals:
    void changeActZone();

private:
    QOpenGLBuffer m_vertexBuf; //vertex buffer for drawing area activating trap
    QOpenGLBuffer m_indexBuf;
    QVector<QVector3D> m_aDrawPoint; // openGL drawing activation direction
    float m_radius;
    CMagicTrap* m_pParent;
};

class CTrapCastPoint : public CObjectBase
{
    Q_OBJECT
public:
    CTrapCastPoint() = delete;
    CTrapCastPoint(CMagicTrap* pTrap);
    CTrapCastPoint(const CTrapCastPoint&) = delete;
    CTrapCastPoint(CMagicTrap* pTrap, const CTrapCastPoint& pCast);
    ~CTrapCastPoint();
    ENodeType nodeType() override {return ENodeType::eTrapCastPoint; }

    void collectlogicParams(QList<QSharedPointer<IPropertyBase>>& aProp, ENodeType paramType) override;
    void applyLogicParam(const QSharedPointer<IPropertyBase>& prop) override;
    void getLogicParam(QSharedPointer<IPropertyBase>& prop, EObjParam propType) override;
    uint deserialize(util::CMobParser& parser) override;
    uint serialize(util::CMobParser& parser) override;
    void deSerializeJsonArray(QJsonArray data);
    void serializeJsonArray(QJsonArray& obj);
    void draw(bool isActive, QOpenGLShaderProgram* program) override;
    bool updatePos(QVector3D& pos) override;
    void markAsDeleted(bool bDeleted = true) override;

signals:
    void changeCastPoint();

private:
    CMagicTrap* m_pParent;
};

class CMagicTrap : public CWorldObj
{
    Q_OBJECT
public:
    CMagicTrap();
    CMagicTrap(const CMagicTrap& trap);
    CMagicTrap(QJsonObject data);
    ~CMagicTrap();
    ENodeType nodeType() override {return ENodeType::eMagicTrap; }

    void draw(bool isActive, QOpenGLShaderProgram* program) override;
    uint deserialize(util::CMobParser& parser) override;
    void serializeJson(QJsonObject& obj) override;
    uint serialize(util::CMobParser& parser) override;
    void collectlogicParams(QList<QSharedPointer<IPropertyBase>>& aProp, ENodeType paramType) override;
    void applyLogicParam(const QSharedPointer<IPropertyBase>& prop) override;
    void getLogicParam(QSharedPointer<IPropertyBase>& prop, EObjParam propType) override;
    void collectParams(QList<QSharedPointer<IPropertyBase>>& aProp, ENodeType paramType) override;
    void getParam(QSharedPointer<IPropertyBase>& prop, EObjParam propType) override;
    void applyParam(const QSharedPointer<IPropertyBase>& prop) override;
    QJsonObject toJson() override;
    void loadFigure() override {}
    void loadTexture() override;
    void collectLogicNodes(QList<CNode*>& arrNode);
    void clearLogicSelect();
    bool updatePos(QVector3D& pos) override;
    void setDrawPosition(QVector3D pos) override final;
    int getZoneId(CActivationZone* pZone);
    CActivationZone* actZoneById(int zoneId);
    int getCastPointId(CTrapCastPoint* pCast);
    CTrapCastPoint* castPointById (int pointId);
    CActivationZone* createActZone();
    void deleteLastActZone();
    CTrapCastPoint* createCastPoint();
    void deleteLastCastPoint();
    const QVector<CActivationZone*>& actZones(){return m_aActZone;}
    const QVector<CTrapCastPoint*>& castPoints() {return m_aCastPoint;}

public slots:
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
