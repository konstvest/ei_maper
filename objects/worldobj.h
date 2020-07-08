#ifndef WORLDOBJ_H
#define WORLDOBJ_H
#include "objects\object_base.h"

class CWorldObj : public CObjectBase
{
public:
    CWorldObj();
    ENodeType nodeType() override {return ENodeType::eWorldObject; }
    uint deserialize(util::CMobParser& parser) override;
    void updateVisibleParts() override;
    QString textureName() override {return m_primaryTexture.toLower();}
    void serializeJson(QJsonObject& obj) override;

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
    QVector<QString> m_bodyParts;
    QString m_parentTemplate;
    //QString m_comment;
    char m_player;
    uint m_parentID;
    bool m_bUseInScript;
    bool m_bShadow;
    QString m_questInfo;
};

#endif // WORLDOBJ_H
