#ifndef WORLDOBJ_H
#define WORLDOBJ_H
#include "objects\object_base.h"

class CMob;

class CWorldObj : public CObjectBase
{
public:
    CWorldObj();
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

private:
    //void init();

protected:
    //uint m_NID;
    uint m_type;
    //QString m_name;
    //QString m_modelName; //moved to object base
    QString m_primaryTexture;
    QString m_secondaryTexture;
    //QVector3D m_pos;
    //QVector4D m_rot;
    //QVector3D m_complection;  //move to object base
    QString m_parentTemplate;
    //QString m_comment;
    char m_player;
    uint m_parentID;
    bool m_bUseInScript;
    bool m_bShadow;
    QString m_questInfo;
    //CMob* m_pMob;
};

#endif // WORLDOBJ_H
