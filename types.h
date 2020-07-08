#ifndef TYPES_H
#define TYPES_H

#include <QVector2D>
#include <QVector3D>
#include "vectors.h"

enum EReadState
{
    eNone = -1
    ,eReadTexture
    ,eReadFigure
    ,eReadLandscape
    ,eReadMob
    ,eReadCount
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

#endif // TYPES_H
