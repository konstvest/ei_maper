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
    return new propStr(type(), pStr->value());
}

bool CPropertyString::isEqual(const IPropertyBase* pProp)
{
    if(const CPropertyString* pValue = dynamic_cast<const CPropertyString*>(pProp))
        return m_value == pValue->value();

    return false;
}

bool CPropertyString::isEqual(const QString str)
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
    return new prop3D(type(), pStr->value());
}

bool CProperty3D::isEqual(const IPropertyBase *pProp)
{
    const prop3D* pData = dynamic_cast<const prop3D*>(pProp);
    if(nullptr != pData)
    {
        const QVector3D& curr = pData->value();
        return curr.x() == m_value.x() && curr.y() == m_value.y() && curr.z() == m_value.z();
    }
    return false;
}

bool CProperty3D::isEqual(const QString str)
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
}

IPropertyBase* CPropertyStringArray::clone() const
{
    const propStrAr* pProp = dynamic_cast<const propStrAr*>(this);
    if(nullptr == pProp)
    {
        Q_ASSERT("incorrect property convertion" && false);
        return nullptr;
    }
    return new propStrAr(type(), pProp->value());
}

bool CPropertyStringArray::isEqual(const IPropertyBase *pProp)
{
    if(const CPropertyStringArray* pValue = dynamic_cast<const CPropertyStringArray*>(pProp))
        return m_value == pValue->value();

    return false;
}

bool CPropertyStringArray::isEqual(const QString str)
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
}

IPropertyBase* CPropertyPartArray::clone() const
{
    const propPart* pProp = dynamic_cast<const propPart*>(this);
    if(nullptr == pProp)
    {
        Q_ASSERT("incorrect property convertion" && false);
        return nullptr;
    }
    return new propPart(type(), pProp->value());
}

bool CPropertyPartArray::isEqual(const IPropertyBase *pProp)
{
    if(const CPropertyPartArray* pValue = dynamic_cast<const CPropertyPartArray*>(pProp))
        return m_value == pValue->value();

    return false;
}

bool CPropertyPartArray::isEqual(const QString str)
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

IPropertyBase* CPropertyStat::clone() const
{
    const propStat* pProp = dynamic_cast<const propStat*>(this);
    if(nullptr == pProp)
    {
        Q_ASSERT("incorrect property convertion" && false);
        return nullptr;
    }
    return new propStat(type(), pProp->value());
}

bool CPropertyStat::isEqual(const IPropertyBase *pProp)
{
    if(const propStat* pValue = dynamic_cast<const propStat*>(pProp))
        return m_value == pValue->value();

    return false;
}

bool CPropertyStat::isEqual(const QString str)
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
}
