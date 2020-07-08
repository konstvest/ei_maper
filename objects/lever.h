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

private:
    char m_curState;
    char m_totalState;
    bool m_bCycled;
    bool m_bCastOnce;
    QVector3D m_stat;
    bool m_bDoor;
    bool m_bRecalcGraph;
};

#endif // LEVER_H
