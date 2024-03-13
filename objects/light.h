#ifndef LIGHT_H
#define LIGHT_H
#include "objects\object_base.h"

class CLight : public CObjectBase
{
public:
    CLight();
    CLight(const CLight& light);
    CLight(QJsonObject data);
    ENodeType nodeType() override {return ENodeType::eLight; }
    uint deserialize(util::CMobParser& parser) override;
    void serializeJson(QJsonObject& obj) override;
    uint serialize(util::CMobParser& parser) override;
    void collectParams(QList<QSharedPointer<IPropertyBase>>& aProp, ENodeType paramType) override;
    void getParam(QSharedPointer<IPropertyBase>& prop, EObjParam propType) override;
    void applyParam(const QSharedPointer<IPropertyBase>& prop) override;
    QJsonObject toJson() override;
    void loadFigure() override {}

private:
    float m_range;
    bool m_bShadow;
    QVector3D m_color;
};

#endif // LIGHT_H
