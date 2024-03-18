#ifndef CDYNAMICCOMBOBOX_H
#define CDYNAMICCOMBOBOX_H

#include <QComboBox>

///
/// \brief The CDynamicComboBox class represents a drop-down list that loads a list of items when clicked.
/// \todo delete the class, since QStringListModel has handled the optimization. (class CComboDynItem)
///
class CDynamicComboBox : public QComboBox
{
    Q_OBJECT
public:
    explicit CDynamicComboBox(QWidget* parent = nullptr);
    virtual ~CDynamicComboBox() {}
    void initComboItem(QString defValue, QStringList items);
    void showPopup() override;

signals:
    void updateValueOver(CDynamicComboBox*);
    void currentIndexChangedOut(QString&);

public slots:
    void currentIndexChangedIn(QString str);

private:
    QStringList m_arrComtoItem;
    QString defaultValue;

};

#endif // CDYNAMICCOMBOBOX_H
