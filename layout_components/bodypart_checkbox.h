#ifndef CBODYPARTCHECKBOX_H
#define CBODYPARTCHECKBOX_H

#include <QCheckBox>

class CBodyPartCheckBox : public QCheckBox
{
    Q_OBJECT
public:
    CBodyPartCheckBox(QWidget *parent = nullptr);
    void nextCheckState() override;
};

#endif // CBODYPARTCHECKBOX_H
