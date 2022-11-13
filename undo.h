#ifndef UNDO_H
#define UNDO_H
#include <QUndoCommand>
#include <QJsonObject>
#include <QMessageBox>
#include "view.h"

class MainWindow;
class CPatrolPoint;
class CLookPoint;
class CUnit;

class COpenCommand: public QUndoCommand, public QObject
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


class CDeleteLogicPoint: public QUndoCommand
{
public:
    enum { Id = 103 };
    CDeleteLogicPoint() = delete;
    CDeleteLogicPoint(CView* pView, QString hash, QUndoCommand *parent = nullptr);

    void undo() override;
    void redo() override;
    //bool mergeWith(const QUndoCommand *command) override;
    int id() const override { return Id; }

private:
    CView* m_pView;
    QString m_nodeHash;
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
    void updateTreeViewSignal();

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
    CChangeLogicParam(CView* pView, QString pointHash, EObjParam objParam, QString value, QUndoCommand *parent = nullptr);

    void undo() override;
    void redo() override;
    //bool mergeWith(const QUndoCommand *command) override;
    int id() const override { return Id; }

signals:
    void updateParam();
    void updatePosOnLand(CNode* pNode);

protected:
    CView* m_pView;
    QString m_pointHash;
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

class CCreateTrapPointCommand: public QUndoCommand
{
public:
    enum { Id = 109 };
    CCreateTrapPointCommand() = delete;
    CCreateTrapPointCommand(CView* pView, uint trapId, bool bActZone, QUndoCommand *parent = nullptr);

    void undo() override;
    void redo() override;
    //bool mergeWith(const QUndoCommand *command) override;
    int id() const override { return Id; }

private:
    CView* m_pView;
    uint m_trapId;
    bool m_bActZone;
};

class CRoundMobCommand: public QUndoCommand
{
public:
    enum { Id = 109 };
    CRoundMobCommand() = delete;
    CRoundMobCommand(CView* pView, QUndoCommand *parent = nullptr);

    void undo() override;
    void redo() override;
    //bool mergeWith(const QUndoCommand *command) override;
    int id() const override { return Id; }

private:
    CView* m_pView;
};

class CCloseActiveMobCommand: public QUndoCommand
{
public:
    enum { Id = 110 };

    CCloseActiveMobCommand(CView* pView, QUndoCommand *parent = nullptr);

    void undo() override;
    void redo() override;
    //bool mergeWith(const QUndoCommand *command) override;
    int id() const override { return Id; }

private:
    CView *m_pView;
    QFileInfo m_filePath;
    MainWindow* m_pMain;

};


class CSwitchToQuestMobCommand: public QObject, public QUndoCommand
{
    Q_OBJECT

public:
    enum { Id = 111 };

    CSwitchToQuestMobCommand(CMob* pMob, QUndoCommand *parent = nullptr);

    void undo() override;
    void redo() override;
    //bool mergeWith(const QUndoCommand *command) override;
    int id() const override { return Id; }

signals:
    void switchQuestMobSignal();

private:
    CMob* m_pMob;
    bool m_bQuestMob;
    QMessageBox::StandardButton m_userAnswer;
    CWorldSet m_oldWS;
    QVector<SRange> m_arrOldMnR;
    QVector<SRange> m_arrOldScR;
    QVector<QVector<uint>> m_oldDiplomacyFoF;
    QVector<QString> m_arrOldDiplomacyFieldName; // can be deleted if use only 'default names from player-0 to player-31
};

class CChangeWorldSetCommand : public QObject, public QUndoCommand
{
    Q_OBJECT
public:
    enum { Id = 112 };

    CChangeWorldSetCommand(CMob* pMob, EWsType paramType, QString& value, QUndoCommand *parent = nullptr);

    void undo() override;
    void redo() override;
    //bool mergeWith(const QUndoCommand *command) override;
    int id() const override { return Id; }

signals:
    void changeWsSignal();

private:
    CMob* m_pMob;
    EWsType m_paramType;
    QString m_oldValue;
    QString m_newValue;
};

class CChangeRangeCommand: public QObject, public QUndoCommand
{
    Q_OBJECT
public:
    enum { Id = 113 };

    CChangeRangeCommand(CMob* pMob, int index, SRange range, QUndoCommand *parent = nullptr);

    void undo() override;
    void redo() override;
    //bool mergeWith(const QUndoCommand *command) override;
    int id() const override { return Id; }

signals:
    void changeRangeSignal();

private:
    CMob *m_pMob;
    int m_index;
    SRange m_oldRange;
    SRange m_newRange;
};

class CChangeDiplomacyTableItem: public QObject, public QUndoCommand
{
    Q_OBJECT
public:
    enum { Id = 114 };

    CChangeDiplomacyTableItem(CMob* pMob, int row, int column, QUndoCommand *parent = nullptr);

    void undo() override;
    void redo() override;
    //bool mergeWith(const QUndoCommand *command) override;
    int id() const override { return Id; }

signals:
    void changeDipGroup(int,int);

private:
    CMob *m_pMob;
    int m_row;
    int m_column;
    int m_oldValue;
    int m_oldValueSymetric;
    bool m_bSymmetric;
};


#endif // UNDO_H
