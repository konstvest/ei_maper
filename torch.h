#ifndef TORCH_H
#define TORCH_H
#include "worldobj.h"

class CTorch : public CWorldObj
{
public:
    CTorch();
    ENodeType nodeType() override {return ENodeType::eTorch; }
    uint deserialize(util::CMobParser& parser) override;

private:
    float m_power;
    QVector3D m_pointLink;
    QString m_sound;
};

#endif // TORCH_H
