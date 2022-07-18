#ifndef WORLDOBJ_H
#define WORLDOBJ_H
#include "objects\object_base.h"

class CMob;

class CWorldObj : public CObjectBase
{
public:
    CWorldObj();
    CWorldObj(const CWorldObj& wo);
    CWorldObj(QJsonObject data);
    ENodeType nodeType() override {return ENodeType::eWorldObject; }
    uint deserialize(util::CMobParser& parser) override;
    QString textureName() override {return m_primaryTexture.toLower();}
    void loadTexture() override;
    void serializeJson(QJsonObject& obj) override;
    uint serialize(util::CMobParser& parser) override;
    void collectParams(QMap<EObjParam, QString>& aParam, ENodeType paramType) override;
    void applyParam(EObjParam param, const QString& value) override;
    QString getParam(EObjParam param) override;
    const char& dipGroup(){return m_player;}
    QJsonObject toJson() override;

protected:
    //uint m_NID;
    uint m_type;
    QString m_primaryTexture;
    QString m_secondaryTexture;
    QString m_parentTemplate;
    char m_player;
    int m_parentID;
    bool m_bUseInScript;
    bool m_bShadow;
    QString m_questInfo;
};

#endif // WORLDOBJ_H
