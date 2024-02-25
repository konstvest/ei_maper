#include "bodypart_checkbox.h"

CBodyPartCheckBox::CBodyPartCheckBox(QWidget *parent) : QCheckBox(parent)
{

}

void CBodyPartCheckBox::nextCheckState()
{
    switch (checkState()) {
    case Qt::CheckState::Checked:
        setCheckState(Qt::CheckState::Unchecked);
        break;
    case Qt::CheckState::Unchecked:
        setCheckState(Qt::CheckState::Checked);
        break;
    case Qt::CheckState::PartiallyChecked:
        setCheckState(Qt::CheckState::Checked);
        break;
    }
}
