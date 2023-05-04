#include "dynamiccombobox.h"

CDynamicComboBox::CDynamicComboBox(QWidget* parent):
    QComboBox(parent)
{
    QObject::connect(this, SIGNAL(currentIndexChanged(QString)), this, SLOT(currentIndexChangedIn(QString))); //reconnect default currentIndexChanged to override
    //QObject::connect(this, SIGNAL(currentTextChanged(QString)), this, SLOT(currentIndexChangedIn(QString)));
}

void CDynamicComboBox::initComboItem(QString defValue, QStringList items)
{
    defaultValue = defValue;
    m_arrComtoItem = items;
    blockSignals(true);
    addItem(defaultValue);
    blockSignals(false);
}

void CDynamicComboBox::showPopup()
{
    blockSignals(true);
    auto curText = currentText();
    clear();
    insertItems(0, m_arrComtoItem);
    if(curText != defaultValue)
        setCurrentText(curText);
    else
    {
        insertItem(0, defaultValue);
        setCurrentIndex(0);
    }
    blockSignals(false);
    QComboBox::showPopup();
}

void CDynamicComboBox::currentIndexChangedIn(QString str)
{
    emit currentIndexChangedOut(str);
}
