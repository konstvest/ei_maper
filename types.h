#ifndef TYPES_H
#define TYPES_H

#include <QVector2D>
#include <QVector3D>
#include <QVector4D>
#include <QDataStream>
#include <QDebug>
#include "vectors.h"

enum ESelectType
{
    eSelectType_Id_range
    ,eSelectType_Map_name
    ,eSelectType_Texture_name
    ,eSelectType_Model_name
    //,eSelectType_Mob_file
    ,eSelectType_Position_circle
    ,eSelectType_Position_rectangle
    ,eSelectType_Diplomacy_group
    ,eSelectType_Database_name
    ,eSelectType_Template
    ,eSelectType_ObjectType
    ,eSelectType_all
};

struct SSelect
{
    ESelectType type;
    //todo: param0 - object filters for find in ENodeType types
    QString param1;
    QString param2;
};

enum EEditMode
{
    eEditModeObjects = 0
    ,eEditModeLogic
    //,eEditModeMpr
    ,eCount
};

enum EOperationType
{
    EOperationTypeObjects = 1
    ,EOperationTypeLogic
    ,EOperationTypeLandscape
};

enum EButtonOp
{
    EButtonOpSelect = 1
    ,EButtonOpMove
    ,EButtonOpRotate
    ,EButtonOpScale
};

enum EReadState
{
    eNone = -1
    ,eReadTexture
    ,eReadFigure
    ,eReadLandscape
    ,eReadMob
    ,eReadCount
};

enum EOperateAxis
{
    EOperateAxisX = 1
    ,EOperateAxisY
    ,EOperateAxisZ
    ,EOperateAxisXY
    ,EOperateAxisXZ
    ,EOperateAxisYZ
};

enum EOperationAxisType
{
    eMove = 1
    ,eRotate
    ,eScale
};

enum EObjParam
{
    eObjParamUnknow = 0

    ,eObjParam_NID

    ,eObjParam_TYPE
    ,eObjParam_TEMPLATE
    ,eObjParam_BODYPARTS
    ,eObjParam_PRIM_TXTR
    ,eObjParam_SEC_TXTR
    ,eObjParam_PLAYER
    ,eObjParam_UNIT_PROTOTYPE

    ,eObjParam_USE_IN_SCRIPT
    ,eObjParam_NAME
    ,eObjParam_QUEST_INFO

    //logic params
    ,eObjParam_AGRESSION_MODE
    ,eObjParam_LOGIC_BEHAVIOUR
    ,eObjParam_GUARD_RADIUS
    ,eObjParam_GUARD_ALARM
    ,eObjParam_GUARD_PLACE
    ,eObjParam_VIEW_WAIT
    ,eObjParam_VIEW_TURN_SPEED

    ,eObjParam_IS_SHADOW
    ,eObjParam_TORCH_PTLINK
    ,eObjParam_TORCH_STRENGHT
    ,eObjParam_TORCH_SOUND

    ,eObjParam_PARENT_TEMPLATE
    ,eObjParam_PARENT_ID

    ,eObjParam_RANGE
    ,eObjParam_SOUND_RESNAME
    ,eObjParam_SOUND_AMBIENT
    ,eObjParam_SOUND_IS_MUSIC
    ,eObjParam_SOUND_RANGE
    ,eObjParam_SOUND_MIN
    ,eObjParam_SOUND_MAX
    ,eObjParam_LIGHT_COLOR
    ,eObjParam_LIGHT_SHADOW

    ,eObjParam_PARTICL_TYPE
    ,eObjParam_PARTICL_SCALE

    //,eObjParam_LEVER_SCIENCE_STATS_NEW
    ,eObjParam_LEVER_SCIENCE_STATS_Type_Open
    ,eObjParam_LEVER_SCIENCE_STATS_Key_ID
    ,eObjParam_LEVER_SCIENCE_STATS_Hands_Sleight
    ,eObjParam_LEVER_TOTAL_STATE
    ,eObjParam_LEVER_CUR_STATE
    ,eObjParam_LEVER_IS_CYCLED
    ,eObjParam_LEVER_IS_DOOR
    ,eObjParam_LEVER_RECALC_GRAPH

    ,eObjParam_TRAP_DIPLOMACY
    ,eObjParam_TRAP_SPELL
    ,eObjParam_TRAP_AREA_RADIUS
    //,eObjParam_TRAP_AREAS
    //,eObjParam_TRAP_TARGETS
    ,eObjParam_TRAP_CAST_ONCE
    ,eObjParam_TRAP_CAST_INTERVAL //is it should not be displayed if cast once == true

    ,eObjParam_UNIT_NEED_IMPORT
    ,eObjParam_UNIT_STATS
    ,eObjParam_UNIT_WEAPONS
    ,eObjParam_UNIT_ARMORS
    ,eObjParam_UNIT_SPELLS
    ,eObjParam_UNIT_QUICK_ITEMS
    ,eObjParam_UNIT_QUEST_ITEMS

    ,eObjParam_COMPLECTION
    ,eObjParam_POSITION
    ,eObjParam_ROTATION

    ,eObjParam_COMMENTS

};

enum EMobType
{
    eEMobTypeQuest
    ,eEMobTypeBase
};

enum ENodeType
{
    eUnknown = 0x0
    ,eBaseType        =0x1    //0000 0001
    ,eWorldObject   =0x3    //0000 0011
    ,eUnit          =0x7    //0000 0111
    ,eTorch         =0xB    //0000 1011
    ,eMagicTrap     =0x13   //0001 0011
    ,eLever         =0x23   //0010 0011
    ,eLight         =0x5    //0000 0101
    ,eSound         =0x9    //0000 1001
    ,eParticle      =0x11   //0001 0001

    ,ePatrolPoint   =0x31   //0011 0001
    ,eLookPoint     =0x51   //0101 0001

    ,eTrapActZone   =0x91 //1001 0001
    ,eTrapCastPoint =0xC1 //1100 0001
};

enum ENodeState
{
    eDraw = 1
    ,eSelect
    ,eHidden
};

enum EWsType
{
    eWsTypeWindDir = 0
    ,eWsTypeWindStr
    ,eWsTypeSunLight
    ,eWsTypeTime
    ,eWsTypeAmbient
    ,eWsTypeCount
};

class CWorldSet
{
public:
    CWorldSet();
    CWorldSet(const CWorldSet& ws);
    void serializeJson(QJsonObject& obj);
    void setData(EWsType type, QString data) {m_arrData[type] = data;}
    const QString& data(EWsType type) {return m_arrData[type];}

private:
    void reset();

private:
    QMap<EWsType, QString> m_arrData;
};

class CBox
{
public:
    CBox() = delete;
    ~CBox(){}
    CBox(const CBox& box);
    CBox(QVector3D minPos, QVector3D maxPos);
    QVector3D center();
    float radius();

private:
    QVector3D m_minPos;
    QVector3D m_maxPos;
    QVector3D m_center;
};

struct SVertexData
{
    SVertexData() {}
    SVertexData(const SVertexData& vertData):
        position(vertData.position)
        ,normal(vertData.normal)
        ,texCoord(vertData.texCoord)
    {}
    SVertexData(QVector3D& pos, QVector3D& nrml, QVector2D& tCoord):
        position(pos)
        ,normal(nrml)
        ,texCoord(tCoord) {}
    SVertexData(QVector3D& pos, QVector4D& nrml, QVector2D& tCoord):
        position(pos)
        ,normal(nrml)
        ,texCoord(tCoord) {}
    QVector3D position;
    QVector3D normal;
    QVector2D texCoord;
};

template <class T> class TValue {
public:
    TValue(){m_bInit = false;}
    TValue(T a){m_value = a; m_bInit = true;}
    T& value() {Q_ASSERT(m_bInit); return m_value;}
    bool isInit() {return m_bInit;}
    void setValue(float val) {m_value = val, m_bInit = true;}

private:
    T m_value;
    bool m_bInit;
};

struct SRange
{
    SRange();
    SRange(const SRange& range) {minRange = range.minRange; maxRange = range.maxRange;}
    SRange& operator= (const SRange& range) {minRange = range.minRange; maxRange = range.maxRange; return *this;}
    bool operator==(const SRange& range) const {return minRange==range.minRange && maxRange==range.maxRange;}
    SRange(uint min, uint max) {minRange = min; maxRange = max;}
    bool isEmpty() {return minRange == 0 && maxRange == 0;}
    uint minRange;
    uint maxRange;
};

struct SArea
{
    QVector2D m_pointTo;
    float m_radius;
};

struct SRectangle
{
    uint m_minX;
    uint m_maxX;
    uint m_minY;
    uint m_maxY;
};

struct SUnitStat
{
    SUnitStat();
    SUnitStat(const SUnitStat& stat);
    SUnitStat(QJsonObject data);
    QJsonObject toJson() const;
    int HP;
    int MaxHP;
    int MP;
    int MaxMP;
    float move;
    float actions;
    float SpeedRun;
    float SpeedWalk;
    float SpeedCrouch;
    float SpeedCrawl;
    float VisionArc;
    float SkillsPeripherial;
    float PeripherialArc;
    float AttackDistance;
    unsigned char AIClassStay;// byte;
    unsigned char AIClassLay;// byte;
    short empty1;// smallint;
    float range;
    float attack;
    float defence;
    float weight;
    float damageMin;
    float damageRange;
    float aImpalling;
    float aSlashing;
    float aCrushing;
    float aThermal;
    float aChemical;
    float aElectrical;
    float aGeneral;
    int absorption;
    float Sight;
    float NightSight;
    float SenseLife;
    float SenseHear;
    float SenseSmell;
    float SenseTracking;
    float pSight;
    float pNightSight;
    float pSenseLife;
    float pSenseHear;
    float pSenseSmell;
    float pSenseTracking;
    unsigned char ManualSkill_SCIENCE;// byte;
    unsigned char ManualSkill_STEALING;// byte;
    unsigned char ManualSkill_TAME;// byte;
    unsigned char MagicalSkill_1;// byte;
    unsigned char MagicalSkill_2;// byte;
    unsigned char MagicalSkill_3;// byte;
    unsigned char empty2;// byte;
    unsigned char empty3;// byte;
};

struct SColor
{
    SColor(){rgb[0]=0; rgb[1]=0; rgb[2]=0;}
    SColor(uchar R, uchar G, uchar B) {rgb[0]=R; rgb[1]=G; rgb[2]=B; rgb[3]=1;}
    SColor(uchar R, uchar G, uchar B, uchar A) { rgb[0]=R; rgb[1]=G; rgb[2]=B; rgb[3]=A; hasAlpha = true; }

    bool hasAlpha = false;
    uchar rgb[4];

    uchar red() const {return rgb[0];}
    uchar red(uchar val) {rgb[0] = val; return rgb[0];}
    uchar green() const  {return rgb[1];}
    uchar green(uchar val) {rgb[1] = val; return rgb[1];}
    uchar blue() const  {return rgb[2];}
    uchar blue(uchar val) {rgb[2] = val; return rgb[2];}
    uchar alpha() const  { return hasAlpha ? rgb[3] : 0;}
    uchar alpha(uchar val){rgb[3] = val; return hasAlpha ? rgb[3] : 0;}

    bool isBlack() const {return rgb[0]==0 && rgb[1]==0 && rgb[2]==0;}

    //todo: operator=
    bool operator==(SColor& color)
    {
        return hasAlpha ?
                    (red() == color.red() && green() == color.green() && blue() == color.blue() && alpha() == color.alpha()) :
                    (red() == color.red() && green() == color.green() && blue() == color.blue());
    }
    bool operator==(const SColor& color) const
    {
        return hasAlpha ?
                    (red() == color.red() && green() == color.green() && blue() == color.blue() && alpha() == color.alpha()) :
                    (red() == color.red() && green() == color.green() && blue() == color.blue());
    }
    QVector4D toVec4()
    {
        return hasAlpha ?
                    QVector4D(rgb[0]/255.0f, rgb[1]/255.0f, rgb[2]/255.0f, rgb[3]/255.0f)  :
                    QVector4D(rgb[0]/255.0f, rgb[1]/255.0f, rgb[2]/255.0f, 1.0f);
    }
};

struct SMmpColorDetail
{
    uint m_value;
    uint m_mask;
    uint m_shift;

    SMmpColorDetail(){}
    SMmpColorDetail(uint val, uint mas, uint shif){m_value = val; m_mask = mas; m_shift = shif;}
    friend QDataStream& operator>> (QDataStream& data, SMmpColorDetail& col)
    {
        return data >> col.m_value >> col.m_mask >> col.m_shift;
    }
};

struct SMmpColor
{
    SMmpColor(){}
    SMmpColor(SMmpColorDetail r, SMmpColorDetail g, SMmpColorDetail b, SMmpColorDetail a)
    {red = r; green = g; blue = b; alpha = a;}

    SMmpColorDetail red;
    SMmpColorDetail green;
    SMmpColorDetail blue;
    SMmpColorDetail alpha;

    friend QDataStream& operator >> (QDataStream& data, SMmpColor& col)
    {
        return data >> col.alpha >> col.red >> col.green >> col.blue;
    }
};

enum eTitleTypeData
{
    eTitleTypeDataUnknown = 0
    ,eTitleTypeDataMpr
    ,eTitleTypeDataActiveMob
    ,eTitleTypeDataDurtyFlag
    ,eTitleTypeDataCount
};

enum EBehaviourType //todo: move to logic m_model
//zone view has these parameters
//idle -BZ
//guard - radius
//patrol - path
//sentry - place
//player - briffing
//guard alarm
{
    eIdle = 0 // ?!
    , eRadius
    , ePath
    , ePlace
    , eBriffing
    , eGuardAlaram

};

typedef ei::vector2<short> S2;
typedef ei::vector2<int> I2;
typedef ei::vector2<uint> UI2;
typedef ei::vector2<float> F2;
typedef QVector2D q_F2;
typedef ei::vector2<double> D2;

typedef ei::vector3<short> S3;
typedef ei::vector3<ushort> US3;
typedef ei::vector3<int> I3;
typedef ei::vector3<float> F3;
typedef QVector3D q_F3;
typedef ei::vector3<double> D3;

typedef ei::vector4<short> S4;
typedef ei::vector4<int> I4;
typedef ei::vector4<float> F4;
typedef ei::vector4<double> D4;
typedef TValue<float> hasFloat;

#endif // TYPES_H
