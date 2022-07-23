#ifndef UNIT_H
#define UNIT_H
#include "objects\worldobj.h"
#include "mob.h"

class CSettings;

class CLookPoint : public CObjectBase
{
    Q_OBJECT
public:
    CLookPoint();
    CLookPoint(const CLookPoint& look);
    //CLookPoint(CNode* node);
    ~CLookPoint() {}
    ENodeType nodeType() override {return ENodeType::eLookPoint;}
    QString getParam(EObjParam param) override;
    void applyParam(EObjParam param, const QString& value) override;
    void collectlogicParams(QMap<EObjParam, QString>& aParam, ENodeType paramType) override final;
    uint deserialize(util::CMobParser& parser) override;
    void serializeJson(QJsonObject &obj) override;
    void deSerializeJson(QJsonObject data);
    uint serialize(util::CMobParser& parser) override;
    bool updatePos(QVector3D& pos) override;

signals:
    void lookPointChanges();

private:
    //QVector3D m_lookPoint;//"ACTION_PT_LOOK_PT", ePlot}; //replaced by CNode pos
    uint m_wait;//"ACTION_PT_WAIT_SEG", eDword};
    uint m_turnSpeed;//"ACTION_PT_TURN_SPEED", eDword};
    char m_flag;//"ACTION_PT_FLAGS", eByte};
};

class CPatrolPoint : public CObjectBase
{
    Q_OBJECT;
public:
    CPatrolPoint();
    CPatrolPoint(const CPatrolPoint& patrol);
    ~CPatrolPoint();
    ENodeType nodeType() override {return ENodeType::ePatrolPoint;}
    void draw(QOpenGLShaderProgram* program) override final;
    void drawSelect(QOpenGLShaderProgram* program = nullptr) override final;
    uint deserialize(util::CMobParser& parser) override final;
    QString getParam(EObjParam param) override;
    void applyParam(EObjParam param, const QString& value) override;
    void collectlogicParams(QMap<EObjParam, QString>& aParam, ENodeType paramType) override final;
    void serializeJson(QJsonObject &obj) override final;
    void deSerializeJson(QJsonObject data);
    uint serialize(util::CMobParser& parser) override final;
    QVector<CLookPoint*>& lookPoint() {return m_aLookPt;}
    void collectLookNodes(QList<CNode*>& arrNode);
    void clearLookSelect();
    bool updatePos(QVector3D& pos) override;

signals:
    void patrolChanges();

public slots:
    void update();


private:
    QOpenGLBuffer m_vertexBuf; //vertex buffer for drawing links between look point and patrol point
    QOpenGLBuffer m_indexBuf;

    //QVector3D m_pos;//"GUARD_PT_POSITION", ePlot}; //replaced by CNode pos
    QVector<CLookPoint*> m_aLookPt;//"GUARD_PT_ACTION", eNull};

    QVector<QVector3D> m_aDrawingLine; // openGL drawing look direction
};

class CUnit;

class CLogic : public QObject
{
    Q_OBJECT
public:
    CLogic() = delete;
    CLogic(const CLogic& logic) = delete;
    CLogic(CUnit* unit, bool bUse=false);
    CLogic(CUnit* unit, const CLogic& logic);
    ~CLogic();
    void draw(QOpenGLShaderProgram* program);
    void drawSelect(QOpenGLShaderProgram* program = nullptr);
    uint deserialize(util::CMobParser& parser);
    void serializeJson(QJsonObject& obj);
    void deSerializeJson(QJsonObject data);
    uint serialize(util::CMobParser& parser);
    bool isUse() {return m_use;}
    void updatePos(QVector3D& dir);
    void collectPatrolNodes(QList<CNode*>& arrNode);
    void clearPatrolSelect();
    void collectlogicParams(QMap<EObjParam, QString>& aParam);
    bool isChild(CPatrolPoint* pPointIn);

public slots:
    void update();

private:
    QOpenGLBuffer m_vertexBuf;
    QOpenGLBuffer m_indexBuf;
    bool m_bCyclic; // true/false
    //uint m_model;//behaviour type (see enum EBehaviourType)
    EBehaviourType m_model;

    //guard point + radius in case of "random" bahavior
    float m_guardRadius; // guard radius
    QVector3D m_guardPlacement; // center point of guarding

    char m_numAlarm; //"UNIT_LOGIC_NALARM", eByte};
    char m_use; // true/false
    float m_wait; // idle time, 15 == 1second
    char m_alarmCondition;//eByte
    float m_help; //alarm radius for calling help
    char m_alwaysActive; // true/false
    char m_agressionMode; //agressive, revenge, fear, fear player
    QVector<CPatrolPoint*> m_aPatrolPt;

    QVector<QVector3D> m_aDrawPoint; //opengl path lines
    CUnit* m_parent;
};

class CUnit: public CWorldObj
{
public:
    CUnit();
    CUnit(const CUnit& unit);
    CUnit(QJsonObject data);
    ~CUnit() override;
    ENodeType nodeType() override {return ENodeType::eUnit; }
    uint deserialize(util::CMobParser& parser) override;
    void draw(QOpenGLShaderProgram* program) override;
    void drawSelect(QOpenGLShaderProgram* program = nullptr) override;
    void serializeJson(QJsonObject& obj) override;
    uint serialize(util::CMobParser& parser) override;
    void collectParams(QMap<EObjParam, QString>& aParam, ENodeType paramType) override;
    void collectlogicParams(QMap<EObjParam, QString>& aParam, ENodeType paramType) override;
    void applyParam(EObjParam param, const QString& value) override;
    QString getParam(EObjParam param) override;
    const QString& databaseName(){return m_prototypeName;}
    bool updatePos(QVector3D& pos) override;
    QJsonObject toJson() override;
    void collectLogicNodes(QList<CNode*>& arrNode);
    void clearLogicSelect();
    bool isChild(CPatrolPoint* pPointIn);

private:
    //"UNIT_R", eNull};
    QString m_prototypeName;//"UNIT_PROTOTYPE", eString};
    //"UNIT_ITEMS", eNull};
    QSharedPointer<SUnitStat> m_stat;//"UNIT_STATS", eUnitStats};
    QVector<QString> m_aQuestItem;//"UNIT_QUEST_ITEMS", eStringArray};
    QVector<QString> m_aQuickItem;//"UNIT_QUICK_ITEMS", eStringArray};
    QVector<QString> m_aSpell;//"UNIT_SPELLS", eStringArray};
    QVector<QString> m_aWeapon;//"UNIT_WEAPONS", eStringArray};
    QVector<QString> m_aArmor;//"UNIT_ARMORS", eStringArray};
    bool m_bImport;//"UNIT_NEED_IMPORT", eByte};
    //
    QVector<CLogic*> m_aLogic;
};

#endif // UNIT_H
