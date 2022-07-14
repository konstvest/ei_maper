#ifndef MAGICTRAP_H
#define MAGICTRAP_H
#include "objects\worldobj.h"
#include "types.h"

class CMagicTrap : public CWorldObj
{
public:
    CMagicTrap();
    CMagicTrap(const CMagicTrap& trap);
    CMagicTrap(QJsonObject data);
    ENodeType nodeType() override {return ENodeType::eMagicTrap; }
    uint deserialize(util::CMobParser& parser) override;
    void serializeJson(QJsonObject& obj) override;
    uint serialize(util::CMobParser& parser) override;
    void collectParams(QMap<EObjParam, QString>& aParam, ENodeType paramType) override;
    void applyParam(EObjParam param, const QString& value) override;
    QString getParam(EObjParam param) override;
    QJsonObject toJson() override;

private:
    int m_diplomacy; //can be -1
    QString m_spell;
    QVector<SArea> m_aArea; // areas where trap will be activated (array of x,y,radius)
    QVector<QVector2D> m_aTarget; //points where trap will cast spell (array of x,y)
    uint m_castInterval;
    bool m_bCastOnce;
};

#endif // MAGICTRAP_H
