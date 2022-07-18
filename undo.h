#ifndef UNDO_H
#define UNDO_H
#include <QUndoCommand>
#include <QJsonObject>
#include "view.h"

class MainWindow;

class COpenCommand: public QUndoCommand
{
public:
    enum { Id = 100 };

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
    CDeleteNodeCommand(CView* pView, uint nodeId, QUndoCommand *parent = nullptr);

    void undo() override;
    void redo() override;
    //bool mergeWith(const QUndoCommand *command) override;
    int id() const override { return Id; }

private:
    CView* m_pView;
    uint m_nodeId;
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
    CChangeStringParam(CView* pView, uint nodeId, EObjParam objParam, QString value, QUndoCommand *parent = nullptr);

    void undo() override;
    void redo() override;
    //bool mergeWith(const QUndoCommand *command) override;
    int id() const override { return Id; }

signals:
    void updateParam();
    void updatePosOnLand(CNode* pNode);

protected:
    CView* m_pView;
    uint m_nodeId;
    EObjParam m_objParam;
    QString m_oldValue;
    QString m_newValue;
};

class CChangeModelParam : public CChangeStringParam
{
    Q_OBJECT
public:
    enum { Id = 103 };

    CChangeModelParam(CView* pView, uint nodeId, EObjParam& objParam, QString value, QUndoCommand *parent = nullptr);

    void undo() override;
    void redo() override;
    //bool mergeWith(const QUndoCommand *command) override;
    int id() const override { return Id; }

private:
    QVector<QString> m_oldBodyparts;
};

class CCreateNodeCommand: public QUndoCommand
{
public:
    enum { Id = 104 };
    CCreateNodeCommand() = delete;
    CCreateNodeCommand(CView* pView, QJsonObject nodeData, QUndoCommand *parent = nullptr);

    void undo() override;
    void redo() override;
    //bool mergeWith(const QUndoCommand *command) override;
    int id() const override { return Id; }

private:
    CView* m_pView;
    QJsonObject m_nodeData;
    uint m_createdNodeId;
};

#endif // UNDO_H
