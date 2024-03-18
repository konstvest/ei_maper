#ifndef CSCRIPTEDITOR_H
#define CSCRIPTEDITOR_H
#include <QPlainTextEdit>


class CLineNumberArea;

///
/// \brief The CScriptEditor class provides script editing *.mob file
///
class CScriptEditor : public QPlainTextEdit
{
    Q_OBJECT
public:
    CScriptEditor(QWidget *parent = 0);
    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int lineNumberAreaWidth();

protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void updateLineNumberAreaWidth(int newBlockCount);
    void highlightCurrentLine();
    void updateLineNumberArea(const QRect &, int);

private:
    CLineNumberArea *m_pLineNumberArea;
};

///
/// \brief The CLineNumberArea class provides a display of the number of lines of the script
///
class CLineNumberArea : public QWidget
{
public:
    CLineNumberArea(CScriptEditor *editor) : QWidget(editor) {
        m_pCodeEditor = editor;
    }

    QSize sizeHint() const override {
        return QSize(m_pCodeEditor->lineNumberAreaWidth(), 0);
    }

protected:
    void paintEvent(QPaintEvent *event) override {
        m_pCodeEditor->lineNumberAreaPaintEvent(event);
    }

private:
    CScriptEditor* m_pCodeEditor;
};

#endif // CSCRIPTEDITOR_H
