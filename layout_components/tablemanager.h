#ifndef CTABLEMANAGER_H
#define CTABLEMANAGER_H
#include <QTableWidget>
#include <QSharedPointer>
#include <QTableWidgetItem>
#include <QComboBox>
#include <QList>
#include <QToolButton>
#include <QSharedPointer>
#include <QLineEdit>

#include "types.h"
#include "undo.h"
#include "property.h"

// any type of single value.
// converting to(from) string must be overrided for each prop
class CValueItem : public QLineEdit
{
    Q_OBJECT
public:
    CValueItem() = delete;
    //CValueItem(EObjParam param); // default 'value dif'
    CValueItem(const QSharedPointer<IPropertyBase>& prop);
    const EObjParam& param(){return m_pValue->type();}
    bool applyChanges(const QString& text);
    const QSharedPointer<IPropertyBase>& value() {return m_pValue;}
    bool onTextChange(CValueItem* pCell);
    void displayValue();

public slots:
    void onTextChangeEnd();

private:
    QSharedPointer<IPropertyBase> m_pValue; //stored value;
};

// any type of single value choosing in initialized list.
// value will converted to(from) string
class CComboItem: public QComboBox
{
    Q_OBJECT
public:
    CComboItem() = delete;
    CComboItem(const QSharedPointer<IPropertyBase>& prop);
    // update value on choosing new one
    const QSharedPointer<IPropertyBase>& value() {return m_pValue;}

public slots:
    void _onChange(QString str); //override default 'on change event'

signals:
    void onValueChange(const QSharedPointer<IPropertyBase>);

private:
    QSharedPointer<IPropertyBase> m_pValue; //stored value;
    QMap<uint, QString> m_valueList;
};

// todo: dynamic combo box (show item on choose)


class CDataItem : public QToolButton
// make "edit" button for changing
{
    Q_OBJECT
public:
    CDataItem() {}; // = delete;
    //CDataItem(IPropertyBase* pProp); //todo: data pointer

private:
    void hello() {};

private:
// part list (QString, value[true,false,undefined])
// unit stats (table with value\different)
// string array (list of strings | "Selected units have different data. Input text will replace data for all selected units")
    //QSharedPointer<IPropertyBase> m_pValue; //stored value;
};

class CColorButtonItem : public QToolButton
{
    Q_OBJECT
public:
    CColorButtonItem() = delete;
    CColorButtonItem(const EObjParam param); // default 'value dif'
    CColorButtonItem(const QSharedPointer<IPropertyBase>& prop);
private:
    void updateColor(const QColor& color);

signals:
    void onColorChange(const QSharedPointer<IPropertyBase>);

public slots:
    void applyColor();

private:
    QSharedPointer<IPropertyBase> m_pValue;

};


// class for management property table (signals trasnfer, show data, trasfer applying changes)
class CTableManager : public QObject
{
    Q_OBJECT
public:
    CTableManager() = delete;
    CTableManager(QTableWidget* pTable);

    void reset();
    void setNewData(const QMap<QSharedPointer<IPropertyBase>, bool>& aProp);

private:
    void initRowName();
    bool isValidValue(const EObjParam param, const QString& value);

signals:
    void onUpdateProperty(const QSharedPointer<IPropertyBase>);

public slots:
    void onParamChange(const QSharedPointer<IPropertyBase> pProp); //retranslate value changing from ui to view class
    void onCellEdit(QTableWidgetItem*); // emulate signal-slot system  for QTableWidget

private:
    QTableWidget* m_pTable;
    QMap<EObjParam, QString> m_aRowName;
    QMap<EObjParam, QMap<uint, QString>> m_comboMap;
};

#endif // CTABLEMANAGER_H
