#ifndef CBODYPARTCHECKBOX_H
#define CBODYPARTCHECKBOX_H

#include <QCheckBox>

///
/// \brief The CBodyPartCheckBox class override QCheckBox for custom behaviour on changing state
///
class CBodyPartCheckBox : public QCheckBox
{
    Q_OBJECT
public:
    CBodyPartCheckBox(QWidget *parent = nullptr) :QCheckBox(parent) {}
    void nextCheckState() override;
};

#endif // CBODYPARTCHECKBOX_H
