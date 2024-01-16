#ifndef UNITSTAT_FORM_H
#define UNITSTAT_FORM_H

#include <QWidget>
#include "property.h"

namespace Ui {
class CUnitStatForm;
}

class CUnitStatForm : public QWidget
{
    Q_OBJECT

public:
    explicit CUnitStatForm(QWidget *parent = nullptr);
    ~CUnitStatForm();
    void setStat(const QSharedPointer<IPropertyBase>& prop);

signals:
    void onApplyChangesSignal(const QSharedPointer<IPropertyBase>&);

private slots:
    void on_pushCancel_clicked();
    void on_pushApply_clicked();

private:
    void reset();

private:
    Ui::CUnitStatForm *ui;
    QSharedPointer<IPropertyBase> m_prop;
};

#endif // UNITSTAT_FORM_H
