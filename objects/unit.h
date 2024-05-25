#ifndef UNIT_H
#define UNIT_H
#include "objects\worldobj.h"
#include "mob\mob.h"

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
    void getLogicParam(QSharedPointer<IPropertyBase>& prop, EObjParam propType) override final;
    void applyLogicParam(const QSharedPointer<IPropertyBase>& prop) override final;
    void collectlogicParams(QList<QSharedPointer<IPropertyBase>>& aProp, ENodeType paramType) override final;
    uint deserialize(util::CMobParser& parser) override;
    void serializeJson(QJsonObject &obj) override;
    void deSerializeJson(QJsonObject data);
    uint serialize(util::CMobParser& parser) override;
    bool updatePos(QVector3D& pos) override;
    void markAsDeleted(bool bDeleted = true) override final;
    CLookPoint* createLookPoint();
    void undo_createLookPoint(CLookPoint* pCreatedPoint);

signals:
    void lookPointChanges();
    void addNewLookPoint(CLookPoint* pBase, CLookPoint* pCreated);
    void undo_addNewLookPoint(CLookPoint* created);

private:
    int m_wait;//"ACTION_PT_WAIT_SEG", eDword};
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
    void draw(bool isActive, QOpenGLShaderProgram* program) override final;
    void drawSelect(QOpenGLShaderProgram* program = nullptr) override final;
    uint deserialize(util::CMobParser& parser) override final;
    void getLogicParam(QSharedPointer<IPropertyBase>& prop, EObjParam propType) override final;
    void applyLogicParam(const QSharedPointer<IPropertyBase>& prop) override final;
    void collectlogicParams(QList<QSharedPointer<IPropertyBase>>& aProp, ENodeType paramType) override final;
    void serializeJson(QJsonObject &obj) override final;
    void deSerializeJson(QJsonObject data);
    uint serialize(util::CMobParser& parser) override final;
    QVector<CLookPoint*>& lookPoint() {return m_aLookPt;}
    void collectLookNodes(QList<CNode*>& arrNode);
    void clearLookSelect();
    bool updatePos(QVector3D& pos) override;
    void markAsDeleted(bool bDeleted = true) override final;
    CPatrolPoint* createNewPoint();
    void undo_createNewPoint(CPatrolPoint* pCreatedPoint);
    void addFirstViewPoint();
    void undo_addFirstViewPoint();
    int getViewId(CLookPoint* pPoint);
    void createViewByIndex(int index);
    void undo_createViewByIndex(int index);
    CLookPoint* viewByIndex(int index);

signals:
    void patrolChanges();
    void addNewPatrolPoint(CPatrolPoint* base, CPatrolPoint* created);
    void undo_addNewPatrolPoint(CPatrolPoint* created);

public slots:
    void update();
    void addNewLookPoint(CLookPoint* pBase, CLookPoint* pCreated);
    void undo_addNewLookPoint(CLookPoint* pCreated);


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
    void draw(bool isActive, QOpenGLShaderProgram* program);
    void drawSelect(QOpenGLShaderProgram* program = nullptr);
    uint deserialize(util::CMobParser& parser);
    void serializeJson(QJsonObject& obj);
    void deSerializeJson(QJsonObject data);
    uint serialize(util::CMobParser& parser);
    bool isUse() {return m_use;}
    void updatePos(QVector3D& offset);
    void collectPatrolNodes(QList<CNode*>& arrNode);
    void clearPatrolSelect();
    void collectlogicParams(QList<QSharedPointer<IPropertyBase>>& aProp);
    void getLogicParam(QSharedPointer<IPropertyBase>& prop, EObjParam propType);
    void applyLogicParam(const QSharedPointer<IPropertyBase>& prop);
    bool isChild(CPatrolPoint* pPointIn);
    void addFirstPoint(QVector3D& pos);
    void undo_addFirstPoint();
    bool isBehaviourPath() {return m_behaviour == EBehaviourType::ePath;}
    int getPatrolId(CPatrolPoint* pPoint);
    void getViewId(int& parentPatrol, int& parentView, CLookPoint* pPoint);
    void createPatrolByIndex(int index);
    void undo_createPatrolByIndex(int index);
    void createViewByIndex(int pointId, int viewId);
    void undo_createViewByIndex(int pointId, int viewId);
    CPatrolPoint* patrolByIndex(int index);
    void setGuardPlacement(QVector3D pos) {m_guardPlacement = pos;}


public slots:
    void createLogicLines();
    void updateLogicLines();
    void recalcPatrolPath();
    void addNewPatrolPoint(CPatrolPoint* base, CPatrolPoint* created);
    void undo_addNewPatrolPoint(CPatrolPoint* pCreated);

private:
    void generateVisibleLogicVBO();
    void drawHelp(bool isActive, QOpenGLShaderProgram* program);

private:
    QOpenGLBuffer m_vertexBuf;
    QOpenGLBuffer m_indexBuf;
    QOpenGLBuffer m_helpVertexBuf;
    QOpenGLBuffer m_helpIndexBuf;
    bool m_bCyclic; // true/false
    //uint m_model;//behaviour type (see enum EBehaviourType)
    EBehaviourType m_behaviour;

    //guard point + radius in case of "random" bahavior
    float m_guardRadius; // guard radius
    QVector3D m_guardPlacement; // center point of guarding

    char m_numAlarm; //"UNIT_LOGIC_NALARM", eByte};
    char m_use; // true/false
    float m_wait; // idle time, 15 == 1second
    char m_alarmCondition;//eByte
    float m_help; //alarm radius for calling help
    char m_alwaysActive; // true/false. this flag ignore inactive state
    char m_agressionMode; //agressive, revenge, fear, fear player
    QVector<CPatrolPoint*> m_aPatrolPt;

    QVector<QVector3D> m_aDrawPoint; //opengl path lines
    QVector<QVector3D> m_aHelpPoint; //opengl path lines
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
    void draw(bool isActive, QOpenGLShaderProgram* program) override;
    void drawSelect(QOpenGLShaderProgram* program = nullptr) override;
    void serializeJson(QJsonObject& obj) override;
    uint serialize(util::CMobParser& parser) override;
    void collectParams(QList<QSharedPointer<IPropertyBase>>& aProp, ENodeType paramType) override;
    void collectlogicParams(QList<QSharedPointer<IPropertyBase>>& aProp, ENodeType paramType) override;
    void getParam(QSharedPointer<IPropertyBase>& prop, EObjParam propType) override;
    void applyParam(const QSharedPointer<IPropertyBase>& prop) override;
    void getLogicParam(QSharedPointer<IPropertyBase>& prop, EObjParam propType) override;
    void applyLogicParam(const QSharedPointer<IPropertyBase>& prop) override final;
    const QString& databaseName(){return m_prototypeName;}
    bool updatePos(QVector3D& pos) override;
    QJsonObject toJson() override;
    void collectLogicNodes(QList<CNode*>& arrNode);
    void clearLogicSelect();
    bool isChild(CPatrolPoint* pPointIn);
    void addFirstPatrolPoint();
    void undo_addFirstPatrolPoint();
    bool isBehaviourPath();
    int getPatrolId(CPatrolPoint* pPoint);
    void getViewId(int& parentPatrol, int& parentView, CLookPoint* pPoint);
    void createPatrolByIndex(int index);
    void undo_createPatrolByIndex(int index);
    void createViewByIndex(int pointId, int viewId);
    void undo_createViewByIndex(int pointId, int viewId);
    CPatrolPoint *patrolByIndex(int index);
    void resetLogic();

private:
    //"UNIT_R", eNull};
    QString m_prototypeName;//"UNIT_PROTOTYPE", eString};
    //"UNIT_ITEMS", eNull};
    QSharedPointer<SUnitStat> m_stat;//"UNIT_STATS", eUnitStats};
    QStringList m_aQuestItem;//"UNIT_QUEST_ITEMS", eStringArray};
    QStringList m_aQuickItem;//"UNIT_QUICK_ITEMS", eStringArray};
    QStringList m_aSpell;//"UNIT_SPELLS", eStringArray};
    QStringList m_aWeapon;//"UNIT_WEAPONS", eStringArray};
    QStringList m_aArmor;//"UNIT_ARMORS", eStringArray};
    bool m_bImport;//"UNIT_NEED_IMPORT", eByte};
    //
    QVector<CLogic*> m_aLogic;
};

#endif // UNIT_H
