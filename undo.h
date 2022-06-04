#ifndef UNDO_H
#define UNDO_H
#include <QUndoCommand>
#include "view.h"

class MainWindow;

class COpenCommand: public QUndoCommand
{
public:
    enum { Id = 101 };

    COpenCommand(CView* pView, QFileInfo& path, MainWindow* pMain, QUndoCommand *parent = nullptr);

    void undo() override;
    void redo() override;
    //bool mergeWith(const QUndoCommand *command) override;
    int id() const override { return Id; }

private:
    CView *m_pView;
    QFileInfo m_filePath;
    MainWindow* m_pMain;

};

class CDeleteNodeCommand: public QUndoCommand
{
public:
    enum { Id = 101 };
    CDeleteNodeCommand() = delete;
    CDeleteNodeCommand(CMob* pMob, CNode* pNode, QUndoCommand *parent = nullptr);

    void undo() override;
    void redo() override;
    //bool mergeWith(const QUndoCommand *command) override;
    int id() const override { return Id; }

private:
    CMob* m_pMob;
    CNode* m_pNode;
};

struct SParam
{
    EObjParam param;
    QString value;
};

class CChangeStringParam : public QObject, public QUndoCommand
{
    Q_OBJECT
public:
    enum { Id = 102 };
    CChangeStringParam();
    CChangeStringParam(CNode* pNode, EObjParam objParam, QString value, QUndoCommand *parent = nullptr);

    void undo() override;
    void redo() override;
    //bool mergeWith(const QUndoCommand *command) override;
    int id() const override { return Id; }

signals:
    void updateParam();
    void updatePosOnLand(CNode* pNode);

protected:
    CNode* m_pNode;
    EObjParam m_objParam;
    QString m_oldValue;
    QString m_newValue;
};

class CChangeModelParam : public CChangeStringParam
{
    Q_OBJECT
public:
    enum { Id = 103 };

    CChangeModelParam(CNode* pNode, EObjParam& objParam, QString value, QUndoCommand *parent = nullptr);

    void undo() override;
    void redo() override;
    //bool mergeWith(const QUndoCommand *command) override;
    int id() const override { return Id; }

private:
    QVector<QString> m_oldBodyparts;
};

#endif // UNDO_H
