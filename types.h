#ifndef TYPES_H
#define TYPES_H

#include <QVector2D>
#include <QVector3D>
#include "vectors.h"

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
    ,eObjParam_BODYPARTS
    ,eObjParam_PLAYER
    ,eObjParam_NID
    ,eObjParam_TYPE
    ,eObjParam_NAME
    ,eObjParam_TEMPLATE
    ,eObjParam_PARENT_TEMPLATE
    ,eObjParam_PRIM_TXTR
    ,eObjParam_SEC_TXTR
    ,eObjParam_COMMENTS
    ,eObjParam_POSITION
    ,eObjParam_ROTATION
    ,eObjParam_USE_IN_SCRIPT
    ,eObjParam_IS_SHADOW
    ,eObjParam_PARENT_ID
    ,eObjParam_QUEST_INFO
    ,eObjParam_COMPLECTION
    ,eObjParam_TORCH_PTLINK
    ,eObjParam_TORCH_STRENGHT
    ,eObjParam_TORCH_SOUND
    ,eObjParam_RANGE
    ,eObjParam_SOUND_RANGE
    ,eObjParam_SOUND_MIN
    ,eObjParam_SOUND_MAX
    ,eObjParam_SOUND_RESNAME
    ,eObjParam_SOUND_AMBIENT
    ,eObjParam_SOUND_IS_MUSIC
    ,eObjParam_LIGHT_SHADOW
    ,eObjParam_LIGHT_COLOR
    ,eObjParam_PARTICL_TYPE
    ,eObjParam_PARTICL_SCALE
    //,eObjParam_LEVER_SCIENCE_STATS_NEW
    ,eObjParam_LEVER_SCIENCE_STATS_Type_Open
    ,eObjParam_LEVER_SCIENCE_STATS_Key_ID
    ,eObjParam_LEVER_SCIENCE_STATS_Hands_Sleight
    //
    ,eObjParam_LEVER_CUR_STATE
    ,eObjParam_LEVER_TOTAL_STATE
    ,eObjParam_LEVER_IS_CYCLED
    ,eObjParam_LEVER_IS_DOOR
    ,eObjParam_LEVER_RECALC_GRAPH
    ,eObjParam_TRAP_DIPLOMACY
    ,eObjParam_TRAP_SPELL
    ,eObjParam_TRAP_AREAS
    ,eObjParam_TRAP_TARGETS
    ,eObjParam_TRAP_CAST_INTERVAL
    ,eObjParam_TRAP_CAST_ONCE
    ,eObjParam_UNIT_NEED_IMPORT
    ,eObjParam_UNIT_PROTOTYPE
    ,eObjParam_UNIT_ARMORS
    ,eObjParam_UNIT_WEAPONS
    ,eObjParam_UNIT_SPELLS
    ,eObjParam_UNIT_QUICK_ITEMS
    ,eObjParam_UNIT_QUEST_ITEMS
    ,eObjParam_UNIT_STATS

};

struct SVertexData
{
    SVertexData() {}
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
    uint minRange;
    uint maxRange;
};

struct SArea
{
    QVector2D m_pointTo;
    uint m_radius;
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
