#ifndef MYGLWIDGET_H
#define MYGLWIDGET_H

#include <QGLWidget>
#include <QOpenGLShader>
#include <QFileInfo>
#include <QTimer>
#include <QSharedPointer>
#include <QTableWidget>

#include "types.h"
#include "selector.h"

class QLineEdit;
class CObjectList;
class CTextureList;
class CKeyManager;
class CMob;
class CCamera;
class CNode;
class CLandscape;
class CKeyManager;
class CSettings;
class CStringItem;
class QUndoStack;
struct SParam;
class CComboBoxItem;
class CMobParameters;
class CProgressView;
class CTableManager;
class COperation;
struct SColor;

class CView : public QGLWidget
{
    Q_OBJECT

public:
    explicit CView(QWidget* parent=nullptr);
    ~CView();

    void loadLandscape(QFileInfo& filePath);
    void unloadLand();
    void loadMob(QFileInfo& filePath);
    void saveMobAs();
    void saveAllMob();
    void unloadMob(QString mobName);
    CLandscape* land() {Q_ASSERT(m_landscape); return m_landscape;}
    bool isLandLoaded() {return nullptr != m_landscape;}
    CTextureList* texList();
    CObjectList* objList();
    void attach(CSettings* pSettings, QTableWidget* pParam, QUndoStack* pStack, CProgressView* pProgress, QLineEdit* pMouseCoord);
    CSettings* settings() {Q_ASSERT(m_pSettings); return m_pSettings;}
    int select(const SSelect& selectParam, bool bAddToSelect = false);
    const QVector<CMob*> mobs() {return m_aMob;}
    void pickObject(QPoint mousePos, bool bAddToSelect);
    void pickObject(const QRect& rect, bool bAddToSelect);
    QVector3D getLandPos(const int cursorPosX, const int cursorPosY);
    void changeOperation(EButtonOp type);
    void operationSetBackup(EOperationAxisType operationType);
    void operationRevert(EOperationAxisType operationType);
    void operationApply(EOperationAxisType operationType);
    void moveTo(QVector3D& dir);
    void rotateTo(QVector3D& rot);
    void scaleTo(QVector3D& scale);
    void deleteSelectedNodes();

protected:
    void initializeGL();
    void initShaders();
    void paintGL();
    void resizeGL(int width, int height);
    void keyPressEvent(QKeyEvent* event);
    void keyReleaseEvent(QKeyEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void wheelEvent(QWheelEvent* event);

private:
    void draw();
    CNode* pickObject(QList<CNode*>& aNode, int x, int y);

    int cauntSelectedNodes();
    void applyParam(SParam& param);
    void getColorFromRect(const QRect& rect, QVector<SColor>& aColor);

public slots:
    void updateWindow();
    void viewParameters();
    void updateReadState(EReadState state); //get signal from reading texture/objects/map/mob
    void onParamChange(SParam& sParam);
    void landPositionUpdate(CNode* pNode);

signals:
    void updateMsg(QString msg);
    void mobLoad(bool bReset);

private:
    int m_height; //for redraw window
    CLandscape* m_landscape;
    QSharedPointer<CCamera> m_cam;
    QOpenGLShaderProgram m_program;
    QOpenGLShaderProgram m_landProgram;
    QOpenGLShaderProgram m_selectProgram;
    QMatrix4x4 m_projection;
    QVector<CMob*> m_aMob;
    QSharedPointer<CObjectList> m_objList;
    QSharedPointer<CTextureList> m_textureList;
    QTimer* m_timer;
    CSettings* m_pSettings;
    QVector<bool> m_aReadState;
    QSharedPointer<CTableManager> m_tableManager;
    QUndoStack* m_pUndoStack;
    CProgressView* m_pProgress;
    QSharedPointer<COperation> m_pOp;
    QMap<CNode*, QVector3D> m_operationBackup;
    EOperationType m_operationType;
};

#endif // MYGLWIDGET_H
