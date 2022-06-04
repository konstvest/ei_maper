#include "table_item.h"


CStringItem::CStringItem(QString value, EObjParam param):
    QTableWidgetItem(value)
{
    m_parameter = param;
}

void initComboStr(QMap<uint, QString>& aStr, const EObjParam param)
{
    switch (param) {


    case eObjParam_LEVER_SCIENCE_STATS_Type_Open:
    {
        aStr[0] = "Not active";
        aStr[1] = "Free";
        aStr[5] = "Hands skill";
        aStr[8] = "Key ID";
        break;
    }
    case eObjParam_PLAYER:
    {
        for(int i(0); i < 32; ++i)
            aStr[i] = "Player-" + QString::number(i);

        break;
    }
    case eObjParam_IS_SHADOW:
    case eObjParam_LEVER_IS_CYCLED:
    case eObjParam_LEVER_IS_DOOR:
    case eObjParam_USE_IN_SCRIPT:
    case eObjParam_LEVER_RECALC_GRAPH:
    {
        aStr[0] = "false";
        aStr[1] = "true";
        break;
    }
    default:
        break;
    }
}

bool isDIfferent(QString& value)
{
    return value == "<different>";
}

QString valueDifferent()
{
    return "<different>";
}

CComboBoxItem::CComboBoxItem(const QString& currentValue, EObjParam param)
{
    m_parameter = param;
    initComboStr(m_aComboString, param);
    int i = 0;
    for (const auto& item : m_aComboString.toStdMap())
    {
        insertItem(i, item.second);
        ++i;
    }

    if(currentValue.isEmpty())
    {
        insertItem(i, valueDifferent());
        setCurrentText(valueDifferent());
    }
    else
        setCurrentText(m_aComboString[currentValue.toUInt()]);
    QObject::connect(this, SIGNAL(currentIndexChanged(QString)), this, SLOT(currentIndexChangedOver(QString))); //reconnect default currentIndexChanged to override
}

void CComboBoxItem::getValue(QString &val)
{
    val = QString::number(m_aComboString.key(currentText()));
}

void CComboBoxItem::currentIndexChangedOver(QString str)
{
    if(isDIfferent(str))
        return; //skip <different value>
    emit updateValueOver(this);
}
