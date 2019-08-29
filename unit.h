#ifndef UNIT_H
#define UNIT_H
#include "worldobj.h"
#include "mob.h"

class CLookPoint : public CObjectBase
{
public:
    CLookPoint();
    CLookPoint(CNode* node);
    ~CLookPoint() {}
//    void draw(QOpenGLShaderProgram* program);
//    void drawSelect(QOpenGLShaderProgram* program = nullptr);
    uint deserialize(util::CMobParser& parser);
    //void attachMob(CMob* mob) {m_mob = mob;}

private:
    //QVector3D m_lookPoint;//"ACTION_PT_LOOK_PT", ePlot}; //replaced by CNode pos
    uint m_wait;//"ACTION_PT_WAIT_SEG", eDword};
    uint m_turnSpeed;//"ACTION_PT_TURN_SPEED", eDword};
    char m_flag;//"ACTION_PT_FLAGS", eByte};
    //CMob* m_mob;

};

class CPatrolPoint: public CObjectBase
{
public:
    CPatrolPoint();
    CPatrolPoint(CNode* node);
    ~CPatrolPoint() override;
    void draw(QOpenGLShaderProgram* program) override;
    void drawSelect(QOpenGLShaderProgram* program = nullptr) override;
    void updateFigure(ei::CFigure* fig) override;
    void setTexture(QOpenGLTexture* texture) override;
    void update();
    uint deserialize(util::CMobParser& parser) override;
    void attachMob(CMob* mob) {m_mob = mob;}

private:
    QOpenGLBuffer m_vertexBuf;
    QOpenGLBuffer m_indexBuf;

    //QVector3D m_pos;//"GUARD_PT_POSITION", ePlot}; //replaced by CNode pos
    QVector<CLookPoint*> m_aLookPt;//"GUARD_PT_ACTION", eNull};

    QVector<QVector3D> m_aDrawingLine; // openGL drawing look direction
    CMob* m_mob;
};

class CUnit;

enum EBehaviourType
{
    eBZ = 0 // ?!
    , eRadius
    , ePath
    , ePlace
    , eBriffing

};

class CLogic
{
public:
    CLogic(CUnit* unit);
    ~CLogic();
    void draw(QOpenGLShaderProgram* program);
    void drawSelect(QOpenGLShaderProgram* program = nullptr);
    uint deserialize(util::CMobParser& parser);
    void updatePointFigure(ei::CFigure* fig);
    void setPointTexture(QOpenGLTexture* pTexture);
    bool isUse() {return m_use;}
    void update();
    void attachMob(CMob* mob) {m_mob = mob;}

private:
    QOpenGLBuffer m_vertexBuf;
    QOpenGLBuffer m_indexBuf;

    //"UNIT_LOGIC", eRecord};
    //"UNIT_LOGIC_AGRESSIV", eNull};
    bool m_bCyclic;//"UNIT_LOGIC_CYCLIC", eByte};
    uint m_model;//"UNIT_LOGIC_MODEL", eDword};

    //guard point + radius in case of "random" bahavior
    float m_guardRadius;//"UNIT_LOGIC_GUARD_R", eFloat};
    QVector3D m_guardPlacement;//"UNIT_LOGIC_GUARD_PT", ePlot};

    char m_numAlarm;//"UNIT_LOGIC_NALARM", eByte};
    char m_use;//"UNIT_LOGIC_USE", eByte};
    //"UNIT_LOGIC_REVENGE", eNull};
    //"UNIT_LOGIC_FEAR", eNull};
    float m_wait;//"UNIT_LOGIC_WAIT", eFloat};
    char m_alarmCondition;//"UNIT_LOGIC_ALARM_CONDITION", eByte};
    float m_help;//"UNIT_LOGIC_HELP", eFloat};
    char m_alwaysActive;//"UNIT_LOGIC_ALWAYS_ACTIVE", eByte};
    char m_agressionMode;//"UNIT_LOGIC_AGRESSION_MODE", eByte};
    QVector<CPatrolPoint*> m_aPatrolPt;

    QVector<QVector3D> m_aDrawPoint; //opengl path lines
    CMob* m_mob;
    CUnit* m_parent;
};

class CUnit: public CWorldObj
{
public:
    CUnit();
    ~CUnit() override;
    ENodeType nodeType() override {return ENodeType::eUnit; }
    uint deserialize(util::CMobParser& parser) override;
    void draw(QOpenGLShaderProgram* program) override;
    void drawSelect(QOpenGLShaderProgram* program = nullptr) override;
    void updateFigure(ei::CFigure* fig) override;
    void setTexture(QOpenGLTexture* texture) override;
    void attachMob(CMob* mob) {m_mob = mob;}

private:
    //"UNIT_R", eNull};
    QString m_prototypeName;//"UNIT_PROTOTYPE", eString};
    //"UNIT_ITEMS", eNull};
    QByteArray m_stat;//"UNIT_STATS", eUnitStats};
    QVector<QString> m_aQuestItem;//"UNIT_QUEST_ITEMS", eStringArray};
    QVector<QString> m_aQuickItem;//"UNIT_QUICK_ITEMS", eStringArray};
    QVector<QString> m_aSpell;//"UNIT_SPELLS", eStringArray};
    QVector<QString> m_aWeapon;//"UNIT_WEAPONS", eStringArray};
    QVector<QString> m_aArmor;//"UNIT_ARMORS", eStringArray};
    bool m_bImport;//"UNIT_NEED_IMPORT", eByte};
    //
    QVector<CLogic*> m_aLogic;
    CMob* m_mob;

};

#endif // UNIT_H
