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

IPropertyBase* CPropertyStat::clone() const
{
    const propStat* pProp = dynamic_cast<const propStat*>(this);
    if(nullptr == pProp)
    {
        Q_ASSERT("incorrect property convertion" && false);
        return nullptr;
    }
    if(m_bInit)
        return new propStat(type(), pProp->value());
    else
        return new propStat(type());
}

bool CPropertyStat::isEqual(const IPropertyBase* pProp) const
{
    if(!isValidProp(pProp, this))
        return false;
    if(const propStat* pValue = dynamic_cast<const propStat*>(pProp))
        return m_value == pValue->value();

    return false;
}

bool CPropertyStat::isEqual(const QString str) const
{
    QSharedPointer<propStat> pData(new propStat(type(), m_value));
    pData->resetFromString(str);
    return isEqual(pData.get());
}

QString CPropertyStat::toString()
{
    QJsonDocument doc(m_value.toJson());
    return QString(doc.toJson(QJsonDocument::Compact));
}

void CPropertyStat::resetFromString(const QString &value)
{
    QJsonDocument doc = QJsonDocument::fromJson(value.toUtf8());
    m_value = SUnitStat(doc.object());
    m_bInit = true;
}

QSharedPointer<IPropertyBase> CPropertyStat::createEmptyCopy()
{
    return QSharedPointer<IPropertyBase>(new CPropertyStat(m_type));
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
