#ifndef TABLE_ITEM_H
#define TABLE_ITEM_H
#include <QTableWidgetItem>
#include <QComboBox>
#include <QList>
#include "types.h"

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

#endif // TABLE_ITEM_H

