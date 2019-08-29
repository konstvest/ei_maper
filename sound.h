#ifndef SOUND_H
#define SOUND_H
#include "object_base.h"
#include "types.h"

class CSound : public CObjectBase
{
public:
    CSound();
    ENodeType nodeType() override {return ENodeType::eSound; }
    uint deserialize(util::CMobParser& parser) override;

private:
    uint m_range;
    uint m_range2;
    uint m_min;
    uint m_max;
    //m_power"SOUND_VOLUME", eNull};
    QVector<QString> m_aResName;
    bool m_bAmbient;
    bool m_bMusic;
};

#endif // SOUND_H
