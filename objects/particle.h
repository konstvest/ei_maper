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
    void collectParams(QMap<EObjParam, QString>& aParam, ENodeType paramType) override;
    void applyParam(EObjParam param, const QString& value) override;
    QString getParam(EObjParam param) override;
    QJsonObject toJson() override;

private:
    uint m_kind;
    float m_scale;
};

#endif // CPARTICLE_H
