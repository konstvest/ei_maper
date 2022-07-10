#ifndef CTABLEMANAGER_H
#define CTABLEMANAGER_H
#include <QTableWidget>
#include <QSharedPointer>
#include <QTableWidgetItem>
#include <QComboBox>
#include <QList>

#include "types.h"
#include "undo.h"

class CStringItem : public QTableWidgetItem
{
public:
    CStringItem(QString value, EObjParam param);
    EObjParam& param(){return m_parameter;}

private:
    EObjParam m_parameter;
};

class CComboBoxItem : public QComboBox
{
Q_OBJECT
public:
    CComboBoxItem(const QString& currentValue, EObjParam param);
    void getValue(QString& val);
    EObjParam& param(){return m_parameter;}
    void subscribe(QTableWidget* pTable);

public slots:
    void currentIndexChangedOver(QString);

signals:
    void updateValueOver(CComboBoxItem*);

private:
    EObjParam m_parameter;
    QMap<uint, QString> m_aComboString;
};

class CTableManager : public QObject
{
    Q_OBJECT
public:
    CTableManager() = delete;
    CTableManager(QTableWidget* pTable);

    void reset();
    void setNewData(QMap<EObjParam, QString>& aParam);

private:
    void initRowName();

signals:
    void changeParamSignal(SParam&);

public slots:
    void onParamChange(CComboBoxItem* pItem);
    void onParamChange(QTableWidgetItem* pItem);


private:
    QTableWidget* m_pTable;
    QMap<EObjParam, QString> m_aRowName;
};

#endif // CTABLEMANAGER_H
