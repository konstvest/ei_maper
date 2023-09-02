#ifndef TORCH_H
#define TORCH_H
#include "objects\worldobj.h"

class CTorch : public CWorldObj
{
public:
    CTorch();
    CTorch(const CTorch& torch);
    CTorch(QJsonObject data);
    ENodeType nodeType() override {return ENodeType::eTorch; }
    uint deserialize(util::CMobParser& parser) override;
    void serializeJson(QJsonObject& obj) override;
    uint serialize(util::CMobParser& parser) override;
    void collectParams(QMap<QSharedPointer<IPropertyBase>, bool>& aProp, ENodeType paramType) override;
    void getParam(QSharedPointer<IPropertyBase>& prop, EObjParam propType) override;
    void applyParam(const QSharedPointer<IPropertyBase>& prop) override;
    QJsonObject toJson() override;

private:
    float m_power;
    QVector3D m_pointLink;
    QString m_sound;
};

#endif // TORCH_H
