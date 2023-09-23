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
#include <QStringListModel>

#include "types.h"
#include "undo.h"
#include "property.h"

class CLineEditEventFilter : public QObject
{
public:
    explicit CLineEditEventFilter(QLineEdit *parent, const QString oldValue):
        QObject(parent), m_value(oldValue)
    {}

    void updateValue(const QString value) {m_value = value;}
    bool eventFilter(QObject* obj, QEvent* e)
    {
        if(e->type() == QEvent::FocusOut)
            restoreValue();

        if (e->type() == QEvent::KeyPress)
        {
            QKeyEvent* keyEvent = static_cast<QKeyEvent*>(e);
            if (keyEvent->key() == Qt::Key_Escape)
                restoreValue();
        }
        // standard event processing
        return QObject::eventFilter(obj, e);
    }

private:
    void restoreValue() {reinterpret_cast<QLineEdit*>(parent())->setText(m_value);}

private:
    QString m_value;
};

// any type of single value.
// converting to(from) string must be overrided for each prop
class CValueItem : public QLineEdit
{
    Q_OBJECT
public:
    CValueItem() = delete;
    CValueItem(const QSharedPointer<IPropertyBase>& prop);

    const EObjParam& param(){return m_pValue->type();}
    bool applyChanges(const QString& text);
    const QSharedPointer<IPropertyBase>& value() {return m_pValue;}

signals:
    void onParamChange(const QSharedPointer<IPropertyBase>&);

public slots:
    void onTextChangeEnd();

private:
    QSharedPointer<IPropertyBase> m_pValue; //stored value;
    QSharedPointer<CLineEditEventFilter> m_filter;
};

// any type of single value choosing in initialized list.
// value will converted to(from) string
class CComboStItem: public QComboBox
{
    Q_OBJECT
public:
    CComboStItem() = delete;
    CComboStItem(const QSharedPointer<IPropertyBase>& prop);
    // update value on choosing new one
    const QSharedPointer<IPropertyBase>& value() {return m_pValue;}

    void wheelEvent(QWheelEvent *e) override
    {
        if(hasFocus())
            QComboBox::wheelEvent(e);
    }

public slots:
    void _onChange(QString str); //override default 'on change event'

signals:
    void onParamChange(const QSharedPointer<IPropertyBase>&);

private:
    QSharedPointer<IPropertyBase> m_pValue; //stored value;
    QMap<uint, QString> m_valueList;
    QSharedPointer<QStringListModel> m_pListModel;
};

class CComboDynItem: public QComboBox
{
    Q_OBJECT
public:
    CComboDynItem() = delete;
    CComboDynItem(const QSharedPointer<IPropertyBase>& prop);
    // update value on choosing new one
    const QSharedPointer<IPropertyBase>& value() {return m_pValue;}

    void wheelEvent(QWheelEvent *e) override
    {
        if(hasFocus())
            QComboBox::wheelEvent(e);
    }

public slots:
    void _onChange(QString str); //override default 'on change event'

signals:
    void onParamChange(const QSharedPointer<IPropertyBase>&);

private:
    QSharedPointer<IPropertyBase> m_pValue; //stored value;
    QStringList m_valueList;
    QSharedPointer<QStringListModel> m_pListModel;
};


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
    void onUpdateProperty(const QSharedPointer<IPropertyBase>); //retranslate value changing from ui to view class

public slots:
    void onParamChange(const QSharedPointer<IPropertyBase>& pProp); //get value for translating it to view class
    void onCellEdit(QTableWidgetItem*); // emulate signal-slot system  for QTableWidget

private:
    QTableWidget* m_pTable;
    QMap<EObjParam, QString> m_aRowName;
    QMap<EObjParam, QMap<uint, QString>> m_comboMap;
};

#endif // CTABLEMANAGER_H
