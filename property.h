#ifndef IPROPERTYBASE_H
#define IPROPERTYBASE_H
#include <QString>
#include <QMap>
#include <QVector3D>
#include <QStringList>
#include "types.h"

class IPropertyBase
{
public:
    IPropertyBase() = delete;
    virtual ~IPropertyBase() {}

    virtual IPropertyBase* clone() const {return nullptr;}
    virtual bool isEqual(const IPropertyBase* pProp) = 0;
    virtual bool isEqual(const QString str) = 0;
    //virtual bool isEqual(const QString value) = 0;
    virtual QString toString() = 0;
    virtual void resetFromString(const QString& value) = 0;

    const EObjParam& type() const {return m_type;}

protected:
    IPropertyBase(EObjParam type): m_type(type) {};

protected:
    EObjParam m_type;
};

class CPropertyString: public IPropertyBase
{
public:
    CPropertyString() = delete;
    IPropertyBase* clone() const override;
    CPropertyString(EObjParam type, const char* value): IPropertyBase(type), m_value(value) {}
    CPropertyString(EObjParam type, const QString& value): IPropertyBase(type), m_value(value) {}
    ~CPropertyString() {}
    bool isEqual(const IPropertyBase* pProp) override final;
    bool isEqual(const QString str) override final;
    //bool isEqual(const QString value) override final;
    QString toString() override final;
    void resetFromString(const QString& value) override final;

    const QString& value() const {return m_value;}
    void setValue(const QString& val) {m_value = val;}
    void setValue(const char* val) {m_value = val;}
private:
    QString m_value;
};

class CPropertyStringArray: public IPropertyBase
{
public:
    CPropertyStringArray() = delete;
    CPropertyStringArray(EObjParam type, const QStringList& list): IPropertyBase(type), m_value(list) {}
    ~CPropertyStringArray() {}
    IPropertyBase* clone() const override;
    bool isEqual(const IPropertyBase* pProp) override final;
    bool isEqual(const QString str) override final;
    QString toString() override final;
    void resetFromString(const QString& value) override final;

    const QStringList& value() const {return m_value;}
private:
    QStringList m_value;
};

class CPropertyPartArray: public IPropertyBase
{
public:
    CPropertyPartArray() = delete;
    CPropertyPartArray(EObjParam type, const QMap<QString, char>& list): IPropertyBase(type), m_value(list) {}
    ~CPropertyPartArray() {}
    IPropertyBase* clone() const override;
    bool isEqual(const IPropertyBase* pProp) override final;
    bool isEqual(const QString str) override final;
    QString toString() override final;
    void resetFromString(const QString& value) override final;

    const QMap<QString, char>& value() const {return m_value;}
private:
    QMap<QString, char> m_value;
};

template<class T> class CPropertyNumber final: public IPropertyBase
{
public:
    CPropertyNumber() = delete;
    CPropertyNumber(EObjParam type, const T value): IPropertyBase(type), m_value(value) {}

    ~CPropertyNumber() override final {};

    IPropertyBase* clone() const override
    {
        const CPropertyNumber<T>* pNum = dynamic_cast<const CPropertyNumber<T>*>(this);
        if(nullptr == pNum)
        {
            Q_ASSERT("incorrect property convertion" && false);
            return nullptr;
        }
        return new CPropertyNumber<T>(type(), pNum->value());
    }

    bool isEqual(const IPropertyBase* pProp) override final
    {
        if(const CPropertyNumber<T>* pValue = dynamic_cast<const CPropertyNumber<T>*>(pProp))
            return m_value == pValue->value();

        return false;
    }

    bool isEqual(const QString str) override final
    {
        return m_value == QVariant(str).value<T>();
    }

    QString toString() override final
    {
        return QString::number(m_value);// QVariant(m_value).toString();
    }

    void resetFromString(const QString& value) override final
    {
        m_value = QVariant(value).value<T>();
    }

    const T& value() const {return m_value;}
protected:
    T m_value;
};

class CProperty3D: public IPropertyBase
{
public:
    CProperty3D() = delete;
    CProperty3D(EObjParam type, const QVector3D& val): IPropertyBase(type), m_value(val) {}
    CProperty3D(EObjParam type, const float x, const float y, const float z): IPropertyBase(type), m_value(x, y, z) {}
    ~CProperty3D() override;

    IPropertyBase* clone() const override;
    bool isEqual(const IPropertyBase* pProp) override final;
    bool isEqual(const QString str) override final;
    QString toString() override final;
    void resetFromString(const QString& value) override final;

    const QVector3D& value() const {return m_value;}
    void setValue(const float x, const float y, const float z) {m_value = QVector3D(x, y, z);}
private:
    QVector3D m_value;
};

class CPropertyStat: public IPropertyBase
{
public:
    CPropertyStat() = delete;
    CPropertyStat(EObjParam type, const SUnitStat& val): IPropertyBase(type), m_value(val) {}
    ~CPropertyStat() {}
    IPropertyBase* clone() const override;
    bool isEqual(const IPropertyBase* pProp) override final;
    bool isEqual(const QString str) override final;
    QString toString() override final;
    void resetFromString(const QString& value) override final;

    const SUnitStat& value() const {return m_value;}
private:
    SUnitStat m_value;
};

typedef CPropertyString propStr;
typedef CPropertyStringArray propStrAr;
typedef CPropertyPartArray propPart;
typedef CProperty3D prop3D;
typedef CPropertyStat propStat;
typedef CPropertyNumber<bool> propBool;
typedef CPropertyNumber<char> propChar;
typedef CPropertyNumber<float> propFloat;
typedef CPropertyNumber<uint> propUint;
typedef CPropertyNumber<int> propInt;
//typedef CPropertyNumber<EBehaviourType> propBehaviour; // use propUint instead

#endif // IPROPERTYBASE_H
