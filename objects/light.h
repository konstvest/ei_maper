#ifndef LIGHT_H
#define LIGHT_H
#include "objects\object_base.h"

class CLight : public CObjectBase
{
public:
    CLight();
    ENodeType nodeType() override {return ENodeType::eLight; }
    uint deserialize(util::CMobParser& parser) override;
    void serializeJson(QJsonObject& obj) override;

private:
    float m_range;
    bool m_bShadow;
    QVector3D m_color;
};

#endif // LIGHT_H
