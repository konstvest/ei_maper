#ifndef SOUND_H
#define SOUND_H
#include "objects\object_base.h"
#include "types.h"

class CSound : public CObjectBase
{
public:
    CSound();
    CSound(const CSound& sound);
    CSound(QJsonObject data);
    ENodeType nodeType() override {return ENodeType::eSound; }
    uint deserialize(util::CMobParser& parser) override;
    void serializeJson(QJsonObject& obj) override;
    uint serialize(util::CMobParser& parser) override;
    void collectParams(QList<QSharedPointer<IPropertyBase>>& aProp, ENodeType paramType) override;
    void getParam(QSharedPointer<IPropertyBase>& prop, EObjParam propType) override;
    void applyParam(const QSharedPointer<IPropertyBase>& prop) override;
    QJsonObject toJson() override;
    void loadFigure() override {}

private:
    uint m_range;
    uint m_range2;
    uint m_min;
    uint m_max;
    //m_power"SOUND_VOLUME", eNull};
    QStringList m_aResName;
    bool m_bAmbient;
    bool m_bMusic;
};

#endif // SOUND_H
