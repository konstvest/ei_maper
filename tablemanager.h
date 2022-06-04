#ifndef CTABLEMANAGER_H
#define CTABLEMANAGER_H
#include <QTableWidget>
#include <QSharedPointer>

#include "types.h"
#include "undo.h"

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
