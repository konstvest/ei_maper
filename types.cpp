#include "types.h"
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include <QVariant>

SUnitStat::SUnitStat():
    HP(0)
    ,MaxHP(0)
    ,MP(0)
    ,MaxMP(0)
    ,move(0.0f)
    ,actions(0.0f)
    ,SpeedRun(0.0f)
    ,SpeedWalk(0.0f)
    ,SpeedCrouch(0.0f)
    ,SpeedCrawl(0.0f)
    ,VisionArc(0.0f)
    ,SkillsPeripherial(0.0f)
    ,PeripherialArc(0.0f)
    ,AttackDistance(0.0f)
    ,AIClassStay('\0')
    ,AIClassLay('\0')
    ,empty1(0)
    ,range(0.0f)
    ,attack(0.0f)
    ,defence(0.0f)
    ,weight(0.0f)
    ,damageMin(0.0f)
    ,damageRange(0.0f)
    ,aImpalling(0.0f)
    ,aSlashing(0.0f)
    ,aCrushing(0.0f)
    ,aThermal(0.0f)
    ,aChemical(0.0f)
    ,aElectrical(0.0f)
    ,aGeneral(0.0f)
    ,absorption(0)
    ,Sight(0.0f)
    ,NightSight(0.0f)
    ,SenseLife(0.0f)
    ,SenseHear(0.0f)
    ,SenseSmell(0.0f)
    ,SenseTracking(0.0f)
    ,pSight(0.0f)
    ,pNightSight(0.0f)
    ,pSenseLife(0.0f)
    ,pSenseHear(0.0f)
    ,pSenseSmell(0.0f)
    ,pSenseTracking(0.0f)
    ,ManualSkill_SCIENCE('\0')
    ,ManualSkill_STEALING('\0')
    ,ManualSkill_TAME('\0')
    ,MagicalSkill_1('\0')
    ,MagicalSkill_2('\0')
    ,MagicalSkill_3('\0')
    ,empty2('\0')
    ,empty3('\0')
{

}

SUnitStat::SUnitStat(const SUnitStat &stat)
{
    HP = stat.HP;
    MaxHP = stat.MaxHP;
    MP = stat.MP;
    MaxMP = stat.MaxMP;
    move = stat.move;
    actions = stat.actions;
    SpeedRun = stat.SpeedRun;
    SpeedWalk = stat.SpeedWalk;
    SpeedCrouch = stat.SpeedCrouch;
    SpeedCrawl = stat.SpeedCrawl;
    VisionArc = stat.VisionArc;
    SkillsPeripherial = stat.SkillsPeripherial;
    PeripherialArc = stat.PeripherialArc;
    AttackDistance = stat.AttackDistance;
    AIClassStay = stat.AIClassStay;
    AIClassLay = stat.AIClassLay;
    empty1 = stat.empty1;
    range = stat.range;
    attack = stat.attack;
    defence = stat.defence;
    weight = stat.weight;
    damageMin = stat.damageMin;
    damageRange = stat.damageRange;
    aImpalling = stat.aImpalling;
    aSlashing = stat.aSlashing;
    aCrushing = stat.aCrushing;
    aThermal = stat.aThermal;
    aChemical = stat.aChemical;
    aElectrical = stat.aElectrical;
    aGeneral = stat.aGeneral;
    absorption = stat.absorption;
    Sight = stat.Sight;
    NightSight = stat.NightSight;
    SenseLife = stat.SenseLife;
    SenseHear = stat.SenseHear;
    SenseSmell = stat.SenseSmell;
    SenseTracking = stat.SenseTracking;
    pSight = stat.pSight;
    pNightSight = stat.pNightSight;
    pSenseLife = stat.pSenseLife;
    pSenseHear = stat.pSenseHear;
    pSenseSmell = stat.pSenseSmell;
    pSenseTracking = stat.pSenseTracking;
    ManualSkill_SCIENCE = stat.ManualSkill_SCIENCE;
    ManualSkill_STEALING = stat.ManualSkill_STEALING;
    ManualSkill_TAME = stat.ManualSkill_TAME;
    MagicalSkill_1 = stat.MagicalSkill_1;
    MagicalSkill_2 = stat.MagicalSkill_2;
    MagicalSkill_3 = stat.MagicalSkill_3;
    empty2 = stat.empty2;
    empty3 = stat.empty3;
}

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
    AIClassLay = (unsigned char)data["AIClassLay"].toVariant().toInt();
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

QJsonObject SUnitStat::toJson() const
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
    obj.insert("AIClassLay", QJsonValue::fromVariant(AIClassLay));// byte;
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

CBox::CBox(const CBox &box):
    m_minPos(box.m_minPos)
  ,m_maxPos(box.m_maxPos)
  ,m_center(box.m_center)
{

}

CBox::CBox(QVector3D minPos, QVector3D maxPos):
    m_minPos(minPos)
  ,m_maxPos(maxPos)
{

}

QVector3D CBox::center()
{
    return QVector3D ((m_minPos + m_maxPos)/2);
}

float CBox::radius()
{
    return (m_maxPos-m_minPos).length();
}


SRange::SRange():
    minRange(0)
    ,maxRange(0)
{

}
