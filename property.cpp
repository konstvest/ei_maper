#include "property.h"
#include "utils.h"
#include <QVariant>
#include <QJsonObject>
#include <QJsonDocument>

IPropertyBase *CPropertyString::clone() const
{
    const propStr* pStr = dynamic_cast<const propStr*>(this);
    if(nullptr == pStr)
    {
        Q_ASSERT("incorrect property convertion" && false);
        return nullptr;
    }
    if(m_bInit)
        return new propStr(type(), pStr->value());
    else
        return new propStr(type());
}

bool CPropertyString::isEqual(const IPropertyBase* pProp) const
{
    if(!isValidProp(pProp, this))
        return false;
    if(const CPropertyString* pValue = dynamic_cast<const CPropertyString*>(pProp))
        return m_value == pValue->value();

    return false;
}

bool CPropertyString::isEqual(const QString str) const
{
    return m_value == str;
}

QString CPropertyString::toString()
{
    return m_value;
}

void CPropertyString::resetFromString(const QString &value)
{
    m_value = value;
    m_bInit = true;
}

QSharedPointer<IPropertyBase> CPropertyString::createEmptyCopy()
{
    return QSharedPointer<IPropertyBase>(new CPropertyString(m_type));
}

CProperty3D::~CProperty3D()
{

}

IPropertyBase* CProperty3D::clone() const
{
    const prop3D* pStr = dynamic_cast<const prop3D*>(this);
    if(nullptr == pStr)
    {
        Q_ASSERT("incorrect property convertion" && false);
        return nullptr;
    }
    if(m_bInit)
        return new prop3D(type(), pStr->value());
    else
        return new prop3D(type());
}

bool CProperty3D::isEqual(const IPropertyBase* pProp) const
{
    if(!isValidProp(pProp, this))
        return false;
    const prop3D* pData = dynamic_cast<const prop3D*>(pProp);
    if(nullptr != pData)
    {
        const QVector3D& curr = pData->value();
        return curr.x() == m_value.x() && curr.y() == m_value.y() && curr.z() == m_value.z();
    }
    return false;
}

bool CProperty3D::isEqual(const QString str) const
{
    QSharedPointer<prop3D> pData(new prop3D(type(), m_value));
    pData->resetFromString(str);
    return isEqual(pData.get());
}

QString CProperty3D::toString()
{
    return util::makeString(m_value);
}

void CProperty3D::resetFromString(const QString &value)
{
    m_value = util::vec3FromString(value);
    m_bInit = true;
}

QSharedPointer<IPropertyBase> CProperty3D::createEmptyCopy()
{
    return QSharedPointer<IPropertyBase>(new CProperty3D(m_type));
}

IPropertyBase* CPropertyStringArray::clone() const
{
    const propStrAr* pProp = dynamic_cast<const propStrAr*>(this);
    if(nullptr == pProp)
    {
        Q_ASSERT("incorrect property convertion" && false);
        return nullptr;
    }
    if(m_bInit)
        return new propStrAr(type(), pProp->value());
    else
        return new propStrAr(type());
}

bool CPropertyStringArray::isEqual(const IPropertyBase* pProp) const
{
    if(!isValidProp(pProp, this))
        return false;
    if(const CPropertyStringArray* pValue = dynamic_cast<const CPropertyStringArray*>(pProp))
        return m_value == pValue->value();

    return false;
}

bool CPropertyStringArray::isEqual(const QString str) const
{
    QSharedPointer<propStrAr> pData(new propStrAr(type(), m_value));
    pData->resetFromString(str);
    return isEqual(pData.get());
}

QString CPropertyStringArray::toString()
{
    return util::makeString(m_value);
}

void CPropertyStringArray::resetFromString(const QString &value)
{
    m_value = util::strArrFromString(value);
    m_bInit = true;
}

QSharedPointer<IPropertyBase> CPropertyStringArray::createEmptyCopy()
{
    return QSharedPointer<IPropertyBase>(new CPropertyStringArray(m_type));
}

IPropertyBase* CPropertyPartArray::clone() const
{
    const propPart* pProp = dynamic_cast<const propPart*>(this);
    if(nullptr == pProp)
    {
        Q_ASSERT("incorrect property convertion" && false);
        return nullptr;
    }
    if(m_bInit)
        return new propPart(type(), pProp->value());
    else
        return new propPart(type());
}

bool CPropertyPartArray::isEqual(const IPropertyBase* pProp) const
{
    if(!isValidProp(pProp, this))
        return false;
    if(const CPropertyPartArray* pValue = dynamic_cast<const CPropertyPartArray*>(pProp))
        return m_value == pValue->value();

    return false;
}

bool CPropertyPartArray::isEqual(const QString str) const
{
    QSharedPointer<propPart> pData(new propPart(type(), m_value));
    pData->resetFromString(str);
    return isEqual(pData.get());
}

QString CPropertyPartArray::toString()
{
    Q_ASSERT(false && "todo: implement");
    return QString();
}

void CPropertyPartArray::resetFromString(const QString &value)
{
    Q_ASSERT(false && "todo: implement");
}

QSharedPointer<IPropertyBase> CPropertyPartArray::createEmptyCopy()
{
    return QSharedPointer<IPropertyBase>(new CPropertyPartArray(m_type));
}

bool IPropertyBase::isValidProp(const IPropertyBase* pPropA, const IPropertyBase* pPropB) const
{
    if(!pPropA->isInit() || !pPropB->isInit())
    {
        ei::log(ELogMessageType::eLogWarning, "prop not initialized");
        return false;
    }
    return true;
}

CPropertyUnitStat::CPropertyUnitStat(EObjParam type, const SUnitStat& val):
  IPropertyBase(type, true)
{
    m_value.resize(51);
    m_value[0].reset(new propInt(eObjParam_UNIT_STATS, val.HP));
    m_value[1].reset(new propInt(eObjParam_UNIT_STATS, val.MaxHP));
    m_value[2].reset(new propInt(eObjParam_UNIT_STATS, val.MP));
    m_value[3].reset(new propInt(eObjParam_UNIT_STATS, val.MaxMP));
    m_value[4].reset(new propFloat(eObjParam_UNIT_STATS, val.move));
    m_value[5].reset(new propFloat(eObjParam_UNIT_STATS, val.actions));
    m_value[6].reset(new propFloat(eObjParam_UNIT_STATS, val.SpeedRun));
    m_value[7].reset(new propFloat(eObjParam_UNIT_STATS, val.SpeedWalk));
    m_value[8].reset(new propFloat(eObjParam_UNIT_STATS, val.SpeedCrouch));
    m_value[9].reset(new propFloat(eObjParam_UNIT_STATS, val.SpeedCrawl));
    m_value[10].reset(new propFloat(eObjParam_UNIT_STATS, val.VisionArc));
    m_value[11].reset(new propFloat(eObjParam_UNIT_STATS, val.SkillsPeripherial));
    m_value[12].reset(new propFloat(eObjParam_UNIT_STATS, val.PeripherialArc));
    m_value[13].reset(new propFloat(eObjParam_UNIT_STATS, val.AttackDistance));
    m_value[14].reset(new propChar(eObjParam_UNIT_STATS, val.AIClassStay));
    m_value[15].reset(new propChar(eObjParam_UNIT_STATS, val.AIClassLay));
    m_value[16].reset(new propChar(eObjParam_UNIT_STATS, val.empty1));
    m_value[17].reset(new propFloat(eObjParam_UNIT_STATS, val.range));
    m_value[18].reset(new propFloat(eObjParam_UNIT_STATS, val.attack));
    m_value[19].reset(new propFloat(eObjParam_UNIT_STATS, val.defence));
    m_value[20].reset(new propFloat(eObjParam_UNIT_STATS, val.weight));
    m_value[21].reset(new propFloat(eObjParam_UNIT_STATS, val.damageMin));
    m_value[22].reset(new propFloat(eObjParam_UNIT_STATS, val.damageRange));
    m_value[23].reset(new propFloat(eObjParam_UNIT_STATS, val.aImpalling));
    m_value[24].reset(new propFloat(eObjParam_UNIT_STATS, val.aSlashing));
    m_value[25].reset(new propFloat(eObjParam_UNIT_STATS, val.aCrushing));
    m_value[26].reset(new propFloat(eObjParam_UNIT_STATS, val.aThermal));
    m_value[27].reset(new propFloat(eObjParam_UNIT_STATS, val.aChemical));
    m_value[28].reset(new propFloat(eObjParam_UNIT_STATS, val.aElectrical));
    m_value[29].reset(new propFloat(eObjParam_UNIT_STATS, val.aGeneral));
    m_value[30].reset(new propInt(eObjParam_UNIT_STATS, val.absorption));
    m_value[31].reset(new propFloat(eObjParam_UNIT_STATS, val.Sight));
    m_value[32].reset(new propFloat(eObjParam_UNIT_STATS, val.NightSight));
    m_value[33].reset(new propFloat(eObjParam_UNIT_STATS, val.SenseLife));
    m_value[34].reset(new propFloat(eObjParam_UNIT_STATS, val.SenseHear));
    m_value[35].reset(new propFloat(eObjParam_UNIT_STATS, val.SenseSmell));
    m_value[36].reset(new propFloat(eObjParam_UNIT_STATS, val.SenseTracking));
    m_value[37].reset(new propFloat(eObjParam_UNIT_STATS, val.pSight));
    m_value[38].reset(new propFloat(eObjParam_UNIT_STATS, val.pNightSight));
    m_value[39].reset(new propFloat(eObjParam_UNIT_STATS, val.pSenseLife));
    m_value[40].reset(new propFloat(eObjParam_UNIT_STATS, val.pSenseHear));
    m_value[41].reset(new propFloat(eObjParam_UNIT_STATS, val.pSenseSmell));
    m_value[42].reset(new propFloat(eObjParam_UNIT_STATS, val.pSenseTracking));
    m_value[43].reset(new propChar(eObjParam_UNIT_STATS, val.ManualSkill_SCIENCE));
    m_value[44].reset(new propChar(eObjParam_UNIT_STATS, val.ManualSkill_STEALING));
    m_value[45].reset(new propChar(eObjParam_UNIT_STATS, val.ManualSkill_TAME));
    m_value[46].reset(new propChar(eObjParam_UNIT_STATS, val.MagicalSkill_1));
    m_value[47].reset(new propChar(eObjParam_UNIT_STATS, val.MagicalSkill_2));
    m_value[48].reset(new propChar(eObjParam_UNIT_STATS, val.MagicalSkill_3));
    m_value[49].reset(new propChar(eObjParam_UNIT_STATS, val.empty2));
    m_value[50].reset(new propChar(eObjParam_UNIT_STATS, val.empty3));
}

IPropertyBase *CPropertyUnitStat::clone() const
{
    const CPropertyUnitStat* pProp = dynamic_cast<const CPropertyUnitStat*>(this);
    if(nullptr == pProp)
    {
        Q_ASSERT("incorrect property convertion" && false);
        return nullptr;
    }
    if(m_bInit)
    {
        CPropertyUnitStat stat(m_type, m_value);
        return new CPropertyUnitStat(type(), pProp->value());
    }
    else
        return new CPropertyUnitStat(type());
}

bool CPropertyUnitStat::isEqual(const IPropertyBase *pProp) const
{
    const CPropertyUnitStat* pData = dynamic_cast<const CPropertyUnitStat*>(pProp);
    const auto& arr = pData->value();
    for(int i(0); i<51; ++i)
    {
        if (!m_value[0]->isEqual(arr[0].get()))
            return false;
    }
    return true;
}

bool CPropertyUnitStat::isEqual(const QString str) const
{
    //todo
    Q_ASSERT(false);
    return false;
}

QString CPropertyUnitStat::toString()
{
    //todo
    return QString("");
}

void CPropertyUnitStat::resetFromString(const QString &value)
{
    //todo
    return;
}

QSharedPointer<IPropertyBase> CPropertyUnitStat::createEmptyCopy()
{
    return QSharedPointer<IPropertyBase>(new CPropertyUnitStat(m_type));
}

void CPropertyUnitStat::resetValue(const QVector<QSharedPointer<IPropertyBase>> &val)
{
    m_value = val;
}
