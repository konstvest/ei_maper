#ifndef CMOBPARAMETERS_H
#define CMOBPARAMETERS_H

#include <QWidget>
#include <QHeaderView>
#include <QTableWidgetItem>
#include <QPlainTextEdit>
#include <QSyntaxHighlighter>
#include <QListWidgetItem>

#include "mob.h"

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

namespace Ui {
class CMobParameters;
}

class CMobParameters : public QWidget
{
    Q_OBJECT
public:
    explicit CMobParameters(QWidget* parent = nullptr, CMob* pMob = nullptr);
    ~CMobParameters();
    void reset();
    void test();

private:
    void updateWindow();
    void convertIdRange();

private slots:
    void onChooseMob(const QString& name);
    void on_diplomacyButton_clicked();
    void tableItemClicked(int r, int c);
    void on_pushCancel_clicked();
    void on_pushApply_clicked();
    void on_isPrimaryBox_clicked();
    void onListItemChanges(QListWidgetItem* pItem);
    void backupItemString(QListWidgetItem* pItem);

    void on_button_minusRanges_clicked();
    void on_button_plusRanges_clicked();

private:
    Ui::CMobParameters *ui;
    QVector<CMob*> m_aMob;
    CMob* m_pCurMob;
    QVector<QSharedPointer<QTableWidgetItem>> m_aCell;
    QSharedPointer<QTableWidget> m_pTable;
    Highlighter* m_pHighlighter;
    QString m_lastItemText;
};

//a header view that renders text vertically
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
