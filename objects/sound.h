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
    void collectParams(QMap<EObjParam, QString>& aParam, ENodeType paramType) override;
    void applyParam(EObjParam param, const QString& value) override;
    QString getParam(EObjParam param) override;
    QJsonObject toJson() override;
    void loadFigure() override {}

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
