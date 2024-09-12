#ifndef CPARTICLE_H
#define CPARTICLE_H
#include "objects\object_base.h"

class CParticle : public CObjectBase
{
public:
    CParticle();
    CParticle(const CParticle& particle);
    CParticle(QJsonObject data);
    ENodeType nodeType() override {return ENodeType::eParticle; }
    uint deserialize(util::CMobParser& parser) override;
    void serializeJson(QJsonObject& obj) override;
    uint serialize(util::CMobParser& parser) override;
    void collectParams(QList<QSharedPointer<IPropertyBase>>& aProp, ENodeType paramType) override;
    void getParam(QSharedPointer<IPropertyBase>& prop, EObjParam propType) override;
    void applyParam(const QSharedPointer<IPropertyBase>& prop) override;
    QJsonObject toJson() override;
    void loadFigure() override {}
    bool isOperationAxisAllow(EOperationAxisType type) override final;

private:
    uint m_kind;
    float m_scale;
};

#endif // CPARTICLE_H
