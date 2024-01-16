#ifndef IPROPERTYBASE_H
#define IPROPERTYBASE_H
#include <QString>
#include <QMap>
#include <QVector3D>
#include <QStringList>
#include "types.h"
#include "log.h"

class IPropertyBase
{
public:
    IPropertyBase() = delete;
    virtual ~IPropertyBase() {}

    virtual IPropertyBase* clone() const {return nullptr;}
    virtual bool isEqual(const IPropertyBase* pProp) const = 0;
    virtual bool isEqual(const QString str) const = 0;
    //virtual bool isEqual(const QString value) = 0;
    virtual QString toString() = 0;
    virtual void resetFromString(const QString& value) = 0;
    virtual QSharedPointer<IPropertyBase> createEmptyCopy() = 0;
    virtual QRegExp validationRegExp() {return QRegExp(".*");};

    const EObjParam& type() const {return m_type;}
    const bool& isInit() const {return m_bInit;}
    void reset() {m_bInit = false;}
    void setInit(bool bInit) {m_bInit = bInit;}

protected:
    IPropertyBase(EObjParam type, bool bInit=false): m_type(type), m_bInit(bInit) {};
    bool isValidProp(const IPropertyBase* pPropA, const IPropertyBase* pPropB) const;

protected:
    EObjParam m_type;
    bool m_bInit;
};

class CPropertyString: public IPropertyBase
{
public:
    CPropertyString() = delete;
    CPropertyString(EObjParam type): IPropertyBase(type) {}
    IPropertyBase* clone() const override;
    CPropertyString(EObjParam type, const char* value): IPropertyBase(type, true), m_value(value) {}
    CPropertyString(EObjParam type, const QString& value): IPropertyBase(type, true), m_value(value) {}
    ~CPropertyString() {}
    bool isEqual(const IPropertyBase* pProp) const override final;
    bool isEqual(const QString str) const override final;
    //bool isEqual(const QString value) override final;
    QString toString() override final;
    void resetFromString(const QString& value) override final;
    QSharedPointer<IPropertyBase> createEmptyCopy() override final;

    const QString& value() const {Q_ASSERT(m_bInit); return m_value;}
    void setValue(const QString& val) {m_value = val;}
    void setValue(const char* val) {m_value = val;}
private:
    QString m_value;
};

class CPropertyStringArray: public IPropertyBase
{
public:
    CPropertyStringArray() = delete;
    CPropertyStringArray(EObjParam type): IPropertyBase(type) {}
    CPropertyStringArray(EObjParam type, const QStringList& list): IPropertyBase(type, true), m_value(list) {}
    ~CPropertyStringArray() {}
    IPropertyBase* clone() const override;
    bool isEqual(const IPropertyBase* pProp) const override final;
    bool isEqual(const QString str) const override final;
    QString toString() override final;
    void resetFromString(const QString& value) override final;
    QSharedPointer<IPropertyBase> createEmptyCopy() override final;

    const QStringList& value() const {Q_ASSERT(m_bInit); return m_value;}
private:
    QStringList m_value;
};

class CPropertyPartArray: public IPropertyBase
{
public:
    CPropertyPartArray() = delete;
    CPropertyPartArray(EObjParam type): IPropertyBase(type) {}
    CPropertyPartArray(EObjParam type, const QMap<QString, char>& list): IPropertyBase(type, true), m_value(list) {}
    ~CPropertyPartArray() {}
    IPropertyBase* clone() const override;
    bool isEqual(const IPropertyBase* pProp) const override final;
    bool isEqual(const QString str) const override final;
    QString toString() override final;
    void resetFromString(const QString& value) override final;
    QSharedPointer<IPropertyBase> createEmptyCopy() override final;

    const QMap<QString, char>& value() const {Q_ASSERT(m_bInit); return m_value;}
private:
    QMap<QString, char> m_value;
};

template<class T> class CPropertyNumber final: public IPropertyBase
{
public:
    CPropertyNumber() = delete;
    CPropertyNumber(EObjParam type): IPropertyBase(type) {}
    CPropertyNumber(EObjParam type, const T value): IPropertyBase(type, true), m_value(value) {}

    ~CPropertyNumber() override final {};

    IPropertyBase* clone() const override
    {
        const CPropertyNumber<T>* pNum = dynamic_cast<const CPropertyNumber<T>*>(this);
        if(nullptr == pNum)
        {
            Q_ASSERT("incorrect property convertion" && false);
            return nullptr;
        }
        if(m_bInit)
            return new CPropertyNumber<T>(type(), pNum->value());
        else
            return new CPropertyNumber<T>(type());

    }

    bool isEqual(const IPropertyBase* pProp) const override final
    {
        if(!isValidProp(pProp, this))
            return false;
        if(const CPropertyNumber<T>* pValue = dynamic_cast<const CPropertyNumber<T>*>(pProp))
            return m_value == pValue->value();

        return false;
    }

    bool isEqual(const QString str) const override final
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
        m_bInit = true;
    }

    QSharedPointer<IPropertyBase> createEmptyCopy() override final
    {
        return QSharedPointer<IPropertyBase>(new CPropertyNumber<T>(m_type));
    }
    QRegExp validationRegExp() override final
    {
        //return QRegExp("\\s*-?\\d+(\\.\\d+)?");
        return QRegExp(R"("^-?\d{1,5}(\.\d{1,5})?$")");
    }

    const T& value() const {Q_ASSERT(m_bInit); return m_value;}
protected:
    T m_value;
};

class CProperty3D: public IPropertyBase
{
public:
    CProperty3D() = delete;
    CProperty3D(EObjParam type): IPropertyBase(type) {}
    CProperty3D(EObjParam type, const QVector3D& val): IPropertyBase(type, true), m_value(val) {}
    CProperty3D(EObjParam type, const float x, const float y, const float z): IPropertyBase(type, true), m_value(x, y, z) {}
    ~CProperty3D() override;

    IPropertyBase* clone() const override;
    bool isEqual(const IPropertyBase* pProp) const override final;
    bool isEqual(const QString str) const override final;
    QString toString() override final;
    void resetFromString(const QString& value) override final;
    QSharedPointer<IPropertyBase> createEmptyCopy() override final;

    const QVector3D& value() const {Q_ASSERT(m_bInit); return m_value;}
    void setValue(const float x, const float y, const float z) {m_value = QVector3D(x, y, z);}
private:
    QVector3D m_value;
};


class CPropertyUnitStat: public IPropertyBase
{
public:
    CPropertyUnitStat() = delete;
    CPropertyUnitStat(EObjParam type): IPropertyBase(type) {m_value.resize(51);}
    CPropertyUnitStat(EObjParam type, const SUnitStat& val);
    CPropertyUnitStat(EObjParam type, const QVector<QSharedPointer<IPropertyBase>>& val): IPropertyBase(type, true), m_value(val) {};
    ~CPropertyUnitStat() {}
    IPropertyBase* clone() const override;
    bool isEqual(const IPropertyBase* pProp) const override final;
    bool isEqual(const QString str) const override final;
    QString toString() override final;
    void resetFromString(const QString& value) override final;
    QSharedPointer<IPropertyBase> createEmptyCopy() override final;

    void resetValue(const QVector<QSharedPointer<IPropertyBase>>& val);

    const QVector<QSharedPointer<IPropertyBase>>& value() const {Q_ASSERT(m_bInit); return m_value;}
private:
    QVector<QSharedPointer<IPropertyBase>> m_value; //51
};

typedef CPropertyString propStr;
typedef CPropertyStringArray propStrAr;
typedef CPropertyPartArray propPart;
typedef CProperty3D prop3D;
typedef CPropertyUnitStat propUnitStat;
typedef CPropertyNumber<bool> propBool;
typedef CPropertyNumber<char> propChar;
typedef CPropertyNumber<float> propFloat;
typedef CPropertyNumber<uint> propUint;
typedef CPropertyNumber<int> propInt;
//typedef CPropertyNumber<EBehaviourType> propBehaviour; // use propUint instead

#endif // IPROPERTYBASE_H
