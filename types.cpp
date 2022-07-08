#include "types.h"
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include <QVariant>

SUnitStat::SUnitStat(QJsonObject data)
{
    HP = data["HP"].toInt();
    MaxHP = data["MaxHP"].toInt();
    MP = data["MP"].toInt();
    MaxMP = data["MaxMP"].toInt();
    move = data["move"].toVariant().toFloat();
    actions = data["actions"].toVariant().toFloat();
    SpeedRun = data["SpeedRun"].toVariant().toFloat();
    SpeedWalk = data["SpeedWalk"].toVariant().toFloat();
    SpeedCrouch = data["SpeedCrouch"].toVariant().toFloat();
    SpeedCrawl = data["SpeedCrawl"].toVariant().toFloat();
    VisionArc = data["VisionArc"].toVariant().toFloat();
    SkillsPeripherial = data["SkillsPeripherial"].toVariant().toFloat();
    PeripherialArc = data["PeripherialArc"].toVariant().toFloat();
    AttackDistance = data["AttackDistance"].toVariant().toFloat();
    AIClassStay = (unsigned char)data["AI Class Stay"].toVariant().toInt();
    AIClassLay = (unsigned char)data[" AIClassLay"].toVariant().toInt();
    empty1 = (short)data["empty1"].toVariant().toInt();
    range = data["range"].toVariant().toFloat();
    attack = data["attack"].toVariant().toFloat();
    defence = data["defence"].toVariant().toFloat();
    weight= data["weight"].toVariant().toFloat();
    damageMin = data["damageMin"].toVariant().toFloat();
    damageRange = data["damageRange"].toVariant().toFloat();
    aImpalling = data["aImpalling"].toVariant().toFloat();
    aSlashing = data["aSlashing"].toVariant().toFloat();
    aCrushing = data["aCrushing"].toVariant().toFloat();
    aThermal = data["aThermal"].toVariant().toFloat();
    aChemical= data["aChemical"].toVariant().toFloat();
    aElectrical = data["aElectrical"].toVariant().toFloat();
    aGeneral = data["aGeneral"].toVariant().toFloat();
    absorption = data["absorption"].toInt();
    Sight = data["Sight"].toVariant().toFloat();
    NightSight = data["NightSight"].toVariant().toFloat();
    SenseLife = data["SenseLife"].toVariant().toFloat();
    SenseHear = data["SenseHear"].toVariant().toFloat();
    SenseSmell = data["SenseSmell"].toVariant().toFloat();
    SenseTracking = data["SenseTracking"].toVariant().toFloat();
    pSight = data["pSight"].toVariant().toFloat();
    pNightSight = data["pNightSight"].toVariant().toFloat();
    pSenseLife = data["pSenseLife"].toVariant().toFloat();
    pSenseHear = data["pSenseHear"].toVariant().toFloat();
    pSenseSmell = data["pSenseSmell"].toVariant().toFloat();
    pSenseTracking = data["pSenseTracking"].toVariant().toFloat();
    ManualSkill_SCIENCE = (unsigned char)data["ManualSkill_SCIENCE"].toVariant().toInt();
    ManualSkill_STEALING = (unsigned char)data["ManualSkill_STEALING"].toVariant().toInt();
    ManualSkill_TAME = (unsigned char)data["ManualSkill_TAME"].toVariant().toInt();
    MagicalSkill_1 = (unsigned char)data["MagicalSkill_1"].toVariant().toInt();
    MagicalSkill_2 = (unsigned char)data["MagicalSkill_2"].toVariant().toInt();
    MagicalSkill_3 = (unsigned char)data["MagicalSkill_3"].toVariant().toInt();
    empty2 = (unsigned char)data["empty2"].toVariant().toInt();
    empty3 = (unsigned char)data["empty3"].toVariant().toInt();
}

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

