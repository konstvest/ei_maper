#ifndef OPTION_H
#define OPTION_H

#include <QString>
#include <QVector>
#include <QMap>

///
/// \brief The COpt class is a base class for working with options. Each option type is overridden below
///
class COpt
{
public:
    //COpt() = delete;
    COpt(QString name);
    virtual ~COpt();
    const QString& name();
    void value() {Q_ASSERT(false);}

private:
    QString m_name;
};

template<typename T> class CTOpt : public COpt
{
public:
    //CTOpt() = delete;
    CTOpt(QString name, const T& val) : COpt(name), m_val(val){}
    virtual ~CTOpt() override {}
    void setValue(T val) {m_val = val;}
    const T& value() const {return m_val;}

private:
    T m_val;
};


typedef CTOpt<bool> COptBool;
typedef CTOpt<QString> COptString;
typedef CTOpt<QVector<QString>> COptStringList;
typedef CTOpt<int> COptInt;
typedef CTOpt<double> COptDouble;

class COptCategory
{
public:
    COptCategory() {}
    ~COptCategory() {}

private:
    QVector<COpt*> m_aOpt;
};

class COptManager
{
public:
    COptManager() {}
    ~COptManager() {}
private:
    QMap<QString, COptCategory*> m_aCategory;
};


#endif // OPTION_H
