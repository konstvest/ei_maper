#ifndef COPERATIONMANAGER_H
#define COPERATIONMANAGER_H

#include <QDebug>
#include <QKeyEvent>

#include "node.h" //todo: move to cpp
#include "types.h"

class CView;
class CState;
class COperation;
class CSelect;
class CMove;
class CRotate;
class CKeyManager;
class CCamera;
class QLineEdit;

///
/// \brief The CState class represents the base class for the keystroke response operation.
///
class CState : public QObject //base state class - interface
{
    Q_OBJECT
  public:
    CState() = delete;
    CState(CView* pView) {m_pView = pView;}
    virtual ~CState() {};
    virtual void keyPress(COperation* pOp, QKeyEvent* event) = 0;
    virtual void keyRelease(COperation* pOp, QKeyEvent* event) = 0;
    virtual void execute(COperation *pOp, QVector3D& dir, CNode* pNode) = 0;
    virtual void axisOperate(COperation *pOp, CNode* pNode) = 0;
    virtual void mousePressEvent(COperation *pOp, QMouseEvent* event) = 0;
    virtual void mouseReleaseEvent(COperation *pOp, QMouseEvent* event) = 0;
    virtual void mouseMoveEvent(COperation *pOp, QMouseEvent* event) = 0;

protected:
    CView* m_pView;
    QPoint m_lastPos;
};

///
/// \brief The COperation class represents an infinite automaton containing the current state(operation type) and allowing to pass between its states.
///
class COperation //state manager
{
public:
    COperation(CState *s);
    void changeState(CState* state);
    void setCurrent(CState *s) {current = s;}
    void keyPress(QKeyEvent* event) {current->keyPress(this, event);};
    void keyRelease(QKeyEvent* event) {current->keyRelease(this, event);};
    void execute(QVector3D& dir, CNode* pNode) {current->execute(this, dir, pNode);}
    void axisOperate(CNode* pNode) {current->axisOperate(this, pNode);};
    void mousePressEvent(QMouseEvent* pEvent);
    void mouseReleaseEvent(QMouseEvent* pEvent);
    void mouseMoveEvent(QMouseEvent* pEvent);

    CKeyManager* keyManager() {return m_keyManager.get();}
    void attachCam(CCamera* pCam);
    CCamera* camera() {return m_pCam;}
    void attachkMouseCoordFiled(QLineEdit* pMouseCoord) {m_pMouseCoord = pMouseCoord;}
    void updateMouseCoords(const QString& pos);

private:
    class CState *current;
    QSharedPointer<CKeyManager> m_keyManager;
    CCamera* m_pCam;
    QLineEdit* m_pMouseCoord;
};

class CSelect: public CState
{
public:
    CSelect(CView* pView);
    ~CSelect() {}
    void keyPress(COperation* pOp, QKeyEvent* event) override;
    void keyRelease(COperation* pOp, QKeyEvent* event) override;
    void execute(COperation *pOp, QVector3D& dir, CNode* pNode) override {Q_UNUSED(pOp);Q_UNUSED(dir);Q_UNUSED(pNode);}
    void axisOperate(COperation *pOp, CNode* pNode) override {Q_UNUSED(pOp);;Q_UNUSED(pNode);};
    void mousePressEvent(COperation *pOp, QMouseEvent* event) override;
    void mouseReleaseEvent(COperation *pOp, QMouseEvent* pEvent) override;
    void mouseMoveEvent(COperation *pOp, QMouseEvent* event) override;

};

class CMoveAxis : public CState
{
    Q_OBJECT
public:
    CMoveAxis() = delete;
    CMoveAxis(CView* pView, EOperateAxis ax);
    ~CMoveAxis() {}

    void keyPress(COperation* pOp, QKeyEvent* event) override final;
    void keyRelease(COperation* pOp, QKeyEvent* event) override final;
    void execute(COperation* pOp, QVector3D& dir, CNode* pNode) override {Q_UNUSED(pOp);Q_UNUSED(dir);Q_UNUSED(pNode);};
    void axisOperate(COperation* pOp, CNode* pNode) override final;
    void mousePressEvent(COperation *pOp, QMouseEvent* event) override final;
    void mouseReleaseEvent(COperation *pOp, QMouseEvent* pEvent) override final;
    void mouseMoveEvent(COperation *pOp, QMouseEvent* event) override final;
private:
    EOperateAxis axis;
    QString value;
    QVector3D lastLandPos;
};

class CRotateAxis : public CState
{
public:
    CRotateAxis() = delete;
    CRotateAxis(CView* pView, EOperateAxis ax);
    ~CRotateAxis() {}
    void keyPress(COperation* pOp, QKeyEvent* event) override;
    void keyRelease(COperation* pOp, QKeyEvent* event) override;
    void execute(COperation* pOp, QVector3D& dir, CNode* pNode) override {Q_UNUSED(pOp);Q_UNUSED(dir);Q_UNUSED(pNode);};
    void axisOperate(COperation* pOp, CNode* pNode) override final;
    void mousePressEvent(COperation *pOp, QMouseEvent* event) override final;
    void mouseReleaseEvent(COperation *pOp, QMouseEvent* pEvent) override final;
    void mouseMoveEvent(COperation *pOp, QMouseEvent* event) override final;
private:
    EOperateAxis axis;
    QString value;
};

class CScaleAxis : public CState
{
public:
    CScaleAxis() = delete;
    CScaleAxis(CView* pView, EOperateAxis ax);
    ~CScaleAxis() {}
    void keyPress(COperation* pOp, QKeyEvent* event) override final;
    void keyRelease(COperation* pOp, QKeyEvent* event) override final;
    void execute(COperation* pOp, QVector3D& dir, CNode* pNode) override {Q_UNUSED(pOp);Q_UNUSED(dir);Q_UNUSED(pNode);};
    void axisOperate(COperation* pOp, CNode* pNode) override;
    void mousePressEvent(COperation *pOp, QMouseEvent* event) override final;
    void mouseReleaseEvent(COperation *pOp, QMouseEvent* pEvent) override final;
    void mouseMoveEvent(COperation *pOp, QMouseEvent* event) override final;
private:
    EOperateAxis axis;
    QString value;
};

#endif // COPERATIONMANAGER_H
