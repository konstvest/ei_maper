#ifndef LEVER_H
#define LEVER_H
#include "objects\worldobj.h"

class CLever : public CWorldObj
{
public:
    CLever();
    ~CLever() override {}
    ENodeType nodeType() override {return ENodeType::eLever; }
    uint deserialize(util::CMobParser& parser) override;
    void serializeJson(QJsonObject& obj) override;
    uint serialize(util::CMobParser& parser) override;
    void collectParams(QMap<EObjParam, QString>& aParam, ENodeType paramType) override;
    void applyParam(EObjParam param, const QString& value) override;
    QString getParam(EObjParam param) override;
    QJsonObject toJson() override;

private:
// maped show these parameters
//    0200 - cur State
//    0300 - total state
//    0400 - cycled
//    0700 - door
//    0800 - recalc graph
    char m_curState;
    char m_totalState;
    bool m_bCycled;
    bool m_bCastOnce;
    uint m_typeOpen;
    uint m_keyID;
    uint m_handsSleight;
    bool m_bDoor;
    bool m_bRecalcGraph;
};

#endif // LEVER_H
