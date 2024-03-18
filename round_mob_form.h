#ifndef ROUND_MOB_FORM_H
#define ROUND_MOB_FORM_H

#include <QWidget>

namespace Ui {
class CRoundMobForm;
}

///
/// \brief The CRoundMobForm class provides possibility to change the currently read *.mob file to the next one
///
class CRoundMobForm : public QWidget
{
    Q_OBJECT

public:
    explicit CRoundMobForm(QWidget *parent = nullptr);
    ~CRoundMobForm();
    void initMobList(QList<QString>& arrMob);
    void round();
    QString selectedMob();

private:
    Ui::CRoundMobForm *ui;
};

#endif // ROUND_MOB_FORM_H
