#include "types.h"
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include <QVariant>

QJsonObject SUnitStat::toJson()
{
    QJsonObject obj;

    obj.insert("HP", HP);
    obj.insert("MaxHP", MaxHP);
    obj.insert("MP", MP);
    obj.insert("MaxMP", MaxMP);
    obj.insert("move", move);
    obj.insert("actions", actions);
    obj.insert("SpeedRun", SpeedRun);
    obj.insert("SpeedWalk", SpeedWalk);
    obj.insert("SpeedCrouch", SpeedCrouch);
    obj.insert("SpeedCrawl", SpeedCrawl);
    obj.insert("VisionArc", VisionArc);
    obj.insert("SkillsPeripherial", SkillsPeripherial);
    obj.insert("PeripherialArc", PeripherialArc);
    obj.insert("AttackDistance", AttackDistance);
    obj.insert("AI Class Stay", QJsonValue::fromVariant(AIClassStay));// byte;
    obj.insert(" AIClassLay", QJsonValue::fromVariant(AIClassLay));// byte;
    obj.insert("empty1", empty1);// smallint;
    obj.insert("range", range);
    obj.insert("attack", attack);
    obj.insert("defence", defence);
    obj.insert("weight", weight);
    obj.insert("damageMin", damageMin);
    obj.insert("damageRange", damageRange);
    obj.insert("aImpalling", aImpalling);
    obj.insert("aSlashing", aSlashing);
    obj.insert("aCrushing", aCrushing);
    obj.insert("aThermal", aThermal);
    obj.insert("aChemical", aChemical);
    obj.insert("aElectrical", aElectrical);
    obj.insert("aGeneral", aGeneral);
    obj.insert("absorption", absorption);
    obj.insert("Sight", Sight);
    obj.insert("NightSight", NightSight);
    obj.insert("SenseLife", SenseLife);
    obj.insert("SenseHear", SenseHear);
    obj.insert("SenseSmell", SenseSmell);
    obj.insert("SenseTracking", SenseTracking);
    obj.insert("pSight", pSight);
    obj.insert("pNightSight", pNightSight);
    obj.insert("pSenseLife", pSenseLife);
    obj.insert("pSenseHear", pSenseHear);
    obj.insert("pSenseSmell", pSenseSmell);
    obj.insert("pSenseTracking", pSenseTracking);
    obj.insert("ManualSkill_SCIENCE", QJsonValue::fromVariant(ManualSkill_SCIENCE));// byte;
    obj.insert("ManualSkill_STEALING", QJsonValue::fromVariant(ManualSkill_STEALING));// byte;
    obj.insert("ManualSkill_TAME", QJsonValue::fromVariant(ManualSkill_TAME));// byte;
    obj.insert("MagicalSkill_1", QJsonValue::fromVariant(MagicalSkill_1));// byte;
    obj.insert("MagicalSkill_2", QJsonValue::fromVariant(MagicalSkill_2));// byte;
    obj.insert("MagicalSkill_3", QJsonValue::fromVariant(MagicalSkill_3));// byte;
    obj.insert("empty2", QJsonValue::fromVariant(empty2));// byte;
    obj.insert("empty3", QJsonValue::fromVariant(empty3));// byte;

    return obj;
}

