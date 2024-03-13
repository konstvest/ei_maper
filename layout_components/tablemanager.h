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

class CLineEditEventFilter;
class CMultiLineEditForm;
class CUnitStatForm;
class CBodyPartEditForm;

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
    void skipNextCheck() {bSkip = true;} //skip checking value for reverting (on lost focus)

signals:
    void onParamChange(const QSharedPointer<IPropertyBase>&);

public slots:
    void onTextChangeEnd();

private:
    QSharedPointer<IPropertyBase> m_pValue; //stored value;
    QSharedPointer<CLineEditEventFilter> m_filter;
    bool bSkip;
};

class CLineEditEventFilter : public QObject
{
public:
    explicit CLineEditEventFilter(CValueItem *parent, const QString oldValue):
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
    void restoreValue();

private:
    QString m_value;
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
    void showPopup() override;

public slots:
    void _onChange(QString str); //override default 'on change event'

signals:
    void onParamChange(const QSharedPointer<IPropertyBase>&);

private:
    QSharedPointer<IPropertyBase> m_pValue; //stored value;
    QStringList m_valueList;
    QSharedPointer<QStringListModel> m_pListModel;
};


class C3DItem: public QWidget
{
    Q_OBJECT
public:
    C3DItem() = delete;
    C3DItem(const QSharedPointer<IPropertyBase>& propX,const QSharedPointer<IPropertyBase>& propY, const QSharedPointer<IPropertyBase>& propZ);
    // update value on choosing new one
    //const QSharedPointer<IPropertyBase>& value() {return m_pValue;}

    void keyPressEvent(QKeyEvent *event) override;

signals:
    void onParamChange(const QSharedPointer<IPropertyBase>&);

public slots:
    void _onParamChange(const QSharedPointer<IPropertyBase>&); // slot for inner valueItem signal. re-translator

private:
    QSharedPointer<IPropertyBase> m_xValue; //stored value;
    QSharedPointer<IPropertyBase> m_yValue; //stored value;
    QSharedPointer<IPropertyBase> m_zValue; //stored value;
};

class CColorButtonItem : public QToolButton
{
    Q_OBJECT
public:
    CColorButtonItem() = delete;
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

class CMultiLineButtonItem: public QToolButton
{
    Q_OBJECT
public:
    CMultiLineButtonItem() = delete;
    CMultiLineButtonItem(const QSharedPointer<IPropertyBase>& prop);

signals:
    void onTextChange(const QSharedPointer<IPropertyBase>);
    void onParamChange(const QSharedPointer<IPropertyBase>&);

public slots:
    void onTextEditOpen();
    void onTextEdit(QString str);

private:
    QSharedPointer<IPropertyBase> m_pValue;
    QSharedPointer<CMultiLineEditForm> m_pTextForm;

};

class CUnitStatItem: public QToolButton
{
    Q_OBJECT
public:
    CUnitStatItem() = delete;
    CUnitStatItem(const QSharedPointer<IPropertyBase>& prop);

signals:
    void onTextChange(const QSharedPointer<IPropertyBase>);
    void onParamChange(const QSharedPointer<IPropertyBase>&);

public slots:
    void onStatEditOpen();
    void _onParamChange(const QSharedPointer<IPropertyBase>& prop); // slot for inner valueItem signal. re-translator

private:
    QSharedPointer<IPropertyBase> m_pValue;
    QSharedPointer<CUnitStatForm> m_pTextForm;

};

class CBodyPartItem: public QToolButton
{
    Q_OBJECT
public:
    CBodyPartItem() = delete;
    CBodyPartItem(const QSharedPointer<IPropertyBase>& prop);

signals:
    void onTextChange(const QSharedPointer<IPropertyBase>);
    void onParamChange(const QSharedPointer<IPropertyBase>&);

public slots:
    void onPartEditOpen();
    void _onParamChange(const QSharedPointer<IPropertyBase>& prop); // slot for inner valueItem signal. re-translator

private:
    QSharedPointer<IPropertyBase> m_pValue;
    QSharedPointer<CBodyPartEditForm> m_pPartForm;

};

// class for management property table (signals trasnfer, show data, trasfer applying changes)
class CTableManager : public QObject
{
    Q_OBJECT
public:
    CTableManager() = delete;
    CTableManager(QTableWidget* pTable);

    void reset();
    void setNewData(const QList<QSharedPointer<IPropertyBase>>& aProp);

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
