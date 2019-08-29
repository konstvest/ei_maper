#ifndef MAGICTRAP_H
#define MAGICTRAP_H
#include "worldobj.h"
#include "types.h"

class CMagicTrap : public CWorldObj
{
public:
    CMagicTrap();
    ENodeType nodeType() override {return ENodeType::eMagicTrap; }
    uint deserialize(util::CMobParser& parser) override;

private:
    uint m_diplomacy;
    QString m_spell;
    QVector<SArea> m_aArea;
    QVector<QVector2D> m_aTarget;
    uint m_castInterval;
    bool m_bCastOnce;
};

#endif // MAGICTRAP_H
