#include "bodypart_checkbox.h"

///
/// \brief Overriding behavior for state selection. You cannot reselect an undefined behavior, only yes/no.
///
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
