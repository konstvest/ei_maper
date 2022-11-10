#ifndef CTABLEMANAGER_H
#define CTABLEMANAGER_H
#include <QTableWidget>
#include <QSharedPointer>
#include <QTableWidgetItem>
#include <QComboBox>
#include <QList>
#include <QToolButton>

#include "types.h"
#include "undo.h"

class CStringItem : public QTableWidgetItem
{
public:
    CStringItem(QString value, EObjParam param);
    EObjParam& param(){return m_parameter;}
    const QString& origValue() {return stored_value;}
    void setNewValue(QString value) {stored_value = value;}

private:
    QString stored_value;
    EObjParam m_parameter;
};

class CComboBoxItem : public QComboBox
{
Q_OBJECT
public:
    CComboBoxItem(const QString& currentValue, EObjParam param);
    void getKey(QString& val);
    void getValue(QString& val);
    EObjParam& param(){return m_parameter;}
    void subscribe(QTableWidget* pTable);
    void showPopup() override;

public slots:
    void currentIndexChangedOver(QString);

signals:
    void updateValueOver(CComboBoxItem*);

private:
    EObjParam m_parameter;
    QMap<uint, QString> m_aComboString;
};

class CColorButtonItem : public QToolButton
{
    Q_OBJECT
public:
    CColorButtonItem(const QString& colorValue, const EObjParam param);
    void updateColor(const QColor& color);
    EObjParam& param() {return m_parameter;}
signals:
    void onColorChange(CColorButtonItem*);
private:
    QColor m_color;
    EObjParam m_parameter;
public slots:
    void colorFromDialog();
};

class CTableManager : public QObject
{
    Q_OBJECT
public:
    CTableManager() = delete;
    CTableManager(QTableWidget* pTable);

    void reset();
    void updateParam(EObjParam param, QString newValue);
    void setNewData(QMap<EObjParam, QString>& aParam);

private:
    void initRowName();
    bool isValidValue(const EObjParam param, const QString& value);

signals:
    void changeParamSignal(SParam&);

public slots:
    void onParamChange(CComboBoxItem* pItem);
    void onParamChange(QTableWidgetItem* pItem);
    void onParamChange(CColorButtonItem* pItem);


private:
    QTableWidget* m_pTable;
    QMap<EObjParam, QString> m_aRowName;
};

#endif // CTABLEMANAGER_H
