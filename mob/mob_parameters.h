#ifndef CMOBPARAMETERS_H
#define CMOBPARAMETERS_H

#include <QWidget>
#include <QHeaderView>
#include <QTableWidgetItem>
#include <QPlainTextEdit>
#include <QUndoStack>
#include <QUndoView>
#include <QSyntaxHighlighter>
#include <QListWidgetItem>
#include <QLineEdit>
#include <QShortcut>

#include "mob.h"

///
/// \brief The Highlighter class provides syntax highlighting for the *.mob file script.
///
class Highlighter : public QSyntaxHighlighter
 {
     Q_OBJECT

 public:
     Highlighter(QTextDocument *parent = 0);

 protected:
     void highlightBlock(const QString &text);

 private:
     struct HighlightingRule
     {
         QRegExp pattern;
         QTextCharFormat format;
     };
     QVector<HighlightingRule> highlightingRules;

     QRegExp commentStartExpression;
     QRegExp commentEndExpression;

     QTextCharFormat keywordFormat;
     QTextCharFormat classFormat;
     QTextCharFormat singleLineCommentFormat;
     QTextCharFormat multiLineCommentFormat;
     QTextCharFormat quotationFormat;
     QTextCharFormat functionFormat;
     QTextCharFormat customFunctionFormat;
     QTextCharFormat typesFormat;
     QTextCharFormat numberFormat;
 };


class CMobParameters;
class CParamLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    CParamLineEdit(QWidget* pParent, EWsType param);
    virtual ~CParamLineEdit() {}
    void saveBackupValue();
    bool isParam(EWsType param) {return param == m_mobParam;}

private slots:
    void editingFinishedOverried();

private:
    bool isValidValue(EWsType paramType, const QString& str);

private:
    CMobParameters* m_pParent;
    QString m_storedValue;
    EWsType m_mobParam;
};

namespace Ui {
class CMobParameters;
}

///
/// \brief The CMobParameters class provides a form for editing parameters of the *mob. file
///
class CMobParameters : public QWidget
{
    Q_OBJECT
public:
    explicit CMobParameters(QWidget* parent = nullptr, CMob* pMob = nullptr, CView* pView = nullptr);
    ~CMobParameters();
    void test();
    void execWsChanges(EWsType paramType, QString& value);
    void setNewRange(SRange& arrRanges, int index);
    const CMob* mob() {return m_pCurMob;}

signals:
    void editFinishedSignal(CMobParameters*); //todo: add signal for pressing 'X'

private:
    void reset();
    void initLineEdit();
    CParamLineEdit* paramLine(EWsType param);
    const QVector<SRange>& activeRanges();
    void setDurty(bool isDurty);
    void generateTitle();

private slots:
    void updateWindow();
    void on_diplomacyButton_clicked();
    void tableItemClicked(int r, int c);
    void on_pushCancel_clicked();
    void on_pushApply_clicked();
    void on_isPrimaryBox_clicked();
    void on_button_minusRanges_clicked();
    void on_button_plusRanges_clicked();
    void onMobUnload(CMob* pMob);
    void on_listRanges_itemDoubleClicked(QListWidgetItem *item);
    void on_pushButtonOpenExtEditor_clicked();
    void updateMobParamsOnly();
    void updateDiplomacyTable();
    void updateDiplomacyTable(int row, int column);
    void on_pushCheck_clicked();
    void onSaveShortcut();
    void showContextMenu(QPoint pos); //context menu for ranges
    void markRange();
    void updateRangeList();

private:
    Ui::CMobParameters *ui;
    CMob* m_pCurMob;
    QVector<QSharedPointer<QTableWidgetItem>> m_aCell;
    QSharedPointer<QTableWidget> m_pTable;
    Highlighter* m_pHighlighter;
    QString m_lastItemText;
    CView* m_pView;
    QSharedPointer<QUndoStack> m_pUndoStack;
    QVector<CParamLineEdit*> m_arrMobParam;
    bool m_isDurty;
    QShortcut* m_pShortcut;
};

///
/// \brief The HorizontalHeaderView class renders text vertically (diplomacy table)
///
class HorizontalHeaderView : public QHeaderView {
public:
    explicit HorizontalHeaderView(Qt::Orientation orientation, QWidget *parent = nullptr)
        : QHeaderView(orientation, parent) {}
//todo: set text to bold when cell is selected
//todo: implement column selected when clicked
    void paintSection(QPainter *painter, const QRect &rect,
                      int logicalIndex) const override {
        QPointF rectCenter = QRectF(rect).center();
        painter->save();
        //rotate around rectCenter
        painter->translate(rectCenter.x(), rectCenter.y());
        painter->rotate(-90.0);
        painter->translate(-rectCenter.x(), -rectCenter.y());
        //apply the same transformation on the rect
        QRect rectCopy = painter->worldTransform().mapRect(rect);
        //use base paintSection method after applying required transformations
        QHeaderView::paintSection(painter, rectCopy, logicalIndex);
        painter->restore();
    }

    QSize sectionSizeFromContents(int logicalIndex) const override {
        //get sizeHint from base sizeHint method
        QSize val = QHeaderView::sectionSizeFromContents(logicalIndex);
        //swap height and width
        return QSize(val.height(), val.width());
    }
};

#endif // CMOBPARAMETERS_H
