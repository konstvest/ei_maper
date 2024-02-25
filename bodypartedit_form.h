#ifndef BODYPARTEDIT_FORM_H
#define BODYPARTEDIT_FORM_H

#include <QWidget>
#include "property.h"

namespace Ui {
class CBodyPartEditForm;
}

class CBodyPartEditForm : public QWidget
{
    Q_OBJECT

public:
    explicit CBodyPartEditForm(QWidget *parent = nullptr);
    ~CBodyPartEditForm();
    void setPartData(const QSharedPointer<IPropertyBase> &prop);

private slots:
    void on_pushCancel_clicked();
    void on_pushApply_clicked();

private:
    void reset();

signals:
    void onApplyChangesSignal(const QSharedPointer<IPropertyBase>&);

private:
    Ui::CBodyPartEditForm *ui;
    QSharedPointer<IPropertyBase> m_prop;
};

#endif // BODYPARTEDIT_FORM_H
