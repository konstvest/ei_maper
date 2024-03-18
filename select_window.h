#ifndef CSELECTOR_H
#define CSELECTOR_H

#include <QWidget>
#include <QMap>
#include <QString>
#include <QStringList>
#include "types.h"

class CView;

namespace Ui {
class CSelectForm;
}

///
/// \brief The CSelectForm class provides a form for defining the parameter by which objects in the 3D scene will be selected after pressing the corresponding key.
///
class CSelectForm : public QWidget
{
    Q_OBJECT
public:
    explicit CSelectForm(QWidget *parent = nullptr);
    void attachParents(CView* pView){m_pView = pView;}
    void onShow();
    void selectAll();

protected:
    void keyPressEvent(QKeyEvent* event);

private:
    void init();
    void updateLabels(const QString& type);
    void updateTotal();

signals:

private slots:
    void on_push_Close_clicked();
    void on_combo_SelType_currentIndexChanged(const QString &arg1);
    void on_push_Select_clicked();
    void on_push_Add_to_select_clicked();

private:
    Ui::CSelectForm *ui;
    CView* m_pView;
    QMap<QString, SSelect> m_loc;
    int m_selected_num;
};

#endif // CSELECTOR_H
