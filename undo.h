#ifndef UNDO_H
#define UNDO_H
#include <QUndoCommand>
#include <QJsonObject>
#include "view.h"

class MainWindow;
class CPatrolPoint;
class CLookPoint;
class CUnit;

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

class CDeletePatrol: public QUndoCommand
{
public:
    enum { Id = 102 };
    CDeletePatrol() = delete;
    CDeletePatrol(CNode* pNode, QUndoCommand *parent = nullptr);

    void undo() override;
    void redo() override;
    int id() const override { return Id; }

private:
    CNode* m_pNode;
};

class CDeleteLogicPoint: public QUndoCommand
{
public:
    enum { Id = 103 };
    CDeleteLogicPoint() = delete;
    CDeleteLogicPoint(CView* pView, uint nodeId, QUndoCommand *parent = nullptr);

    void undo() override;
    void redo() override;
    //bool mergeWith(const QUndoCommand *command) override;
    int id() const override { return Id; }

private:
    CView* m_pView;
    CNode* m_pPoint;
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
    enum { Id = 104 };
    CChangeStringParam() = delete;
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
    enum { Id = 105 };

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
    enum { Id = 106 };
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

class CChangeLogicParam : public QObject, public QUndoCommand
{
    Q_OBJECT
public:
    enum { Id = 107 };
    CChangeLogicParam() = delete;
    CChangeLogicParam(CView* pView, CNode* pPoint, EObjParam objParam, QString value, QUndoCommand *parent = nullptr);

    void undo() override;
    void redo() override;
    //bool mergeWith(const QUndoCommand *command) override;
    int id() const override { return Id; }

signals:
    void updateParam();
    void updatePosOnLand(CNode* pNode);

protected:
    CView* m_pView;
    CNode* m_pPoint;
    EObjParam m_objParam;
    QString m_oldValue;
    QString m_newValue;
};



class CCreatePatrolCommand: public QUndoCommand
{
public:
    enum { Id = 108 };
    CCreatePatrolCommand() = delete;
    CCreatePatrolCommand(CView* pView, QString pointHash, QUndoCommand *parent = nullptr);

    void undo() override;
    void redo() override;
    //bool mergeWith(const QUndoCommand *command) override;
    int id() const override { return Id; }

private:
    CView* m_pView;
    QString m_pointHash;
};

//class CCreateUnitPatrolCommand: public QUndoCommand
//{
//public:
//    enum { Id = 109 };
//    CCreateUnitPatrolCommand() = delete;
//    CCreateUnitPatrolCommand(CView* pView, CUnit* pUnit, QUndoCommand *parent = nullptr);

//    void undo() override;
//    void redo() override;
//    //bool mergeWith(const QUndoCommand *command) override;
//    int id() const override { return Id; }

//private:
//    CView* m_pView;
//    CUnit* m_pUnit;
//};

//class CCreateViewCommand: public QUndoCommand
//{
//public:
//    enum { Id = 110 };
//    CCreateViewCommand() = delete;
//    CCreateViewCommand(CView* pView, CLookPoint* pBasePoint, QUndoCommand *parent = nullptr);

//    void undo() override;
//    void redo() override;
//    //bool mergeWith(const QUndoCommand *command) override;
//    int id() const override { return Id; }

//private:
//    CView* m_pView;
//    CLookPoint* m_pBasePoint;
//    CLookPoint* m_pCreatedPoint;
//};

//class CCreatePatrolViewCommand: public QUndoCommand
//{
//public:
//    enum { Id = 111 };
//    CCreatePatrolViewCommand() = delete;
//    CCreatePatrolViewCommand(CView* pView, CPatrolPoint* pPoint, QUndoCommand *parent = nullptr);

//    void undo() override;
//    void redo() override;
//    //bool mergeWith(const QUndoCommand *command) override;
//    int id() const override { return Id; }

//private:
//    CView* m_pView;
//    CPatrolPoint* m_pPoint;
//};

#endif // UNDO_H
