#ifndef MYGLWIDGET_H
#define MYGLWIDGET_H

#include <QGLWidget>
#include <QOpenGLShader>
#include <QFileInfo>
#include <QTimer>
#include <QSharedPointer>
#include <QTableWidget>
#include <QTreeWidgetItem>
#include <QDateTime>

#include "types.h"
#include "select_window.h"

class QLineEdit;
class CKeyManager;
class CMob;
class CCamera;
class CNode;
class CKeyManager;
class CSettings;
class CStringItem;
class QUndoStack;
struct SParam;
class CComboBoxItem;
class CProgressView;
class CTableManager;
class COperation;
struct SColor;
class CSelectFrame;
class QTreeWidget;
class CMobParameters;
class CRoundMobForm;
class CTreeView;
class IPropertyBase;
class CTileForm;
class CPreviewTile;
class CLandscape;

///
/// \brief The CView class is the main class for managing and editing 3D scene contents. It is also a link between read out *.mob files and their editing. It also controls the camera and current operations.
/// \todo Separate "core" functionality from rendering so that you can use the editor without using the UI
///
class CView : public QGLWidget
{
    Q_OBJECT

public:
    explicit CView(QWidget* parent=nullptr) = delete;
    explicit CView(QWidget* parent=nullptr, const QGLWidget* pShareWidget=nullptr);
    ~CView();

    void loadLandscape(const QFileInfo& filePath);
    void unloadLand();
    void saveLand();
    void saveLandAs();
    void loadMob(QFileInfo& filePath);
    void saveMobAs();
    void saveActiveMob();
    void saveAllMob();
    void unloadActiveMob();
    void openActiveMobEditParams();
    void unloadMob(QString mobName);
    void attach(CSettings* pSettings, QTableWidget* pParam, QUndoStack* pStack, CProgressView* pProgress, QLineEdit* pMouseCoord, CTreeView* pTree, CTileForm* pTileForm);
    CSettings* settings() {Q_ASSERT(m_pSettings); return m_pSettings;}
    int select(const SSelect& selectParam, bool bAddToSelect = false);
    CMob* mob(QString mobName);
    const QVector<CMob*> mobs() {return m_aMob;}
    void drawSelectFrame(QRect& rect);
    void pickObject(QPoint mousePos, bool bAddToSelect);
    void pickObject(const QRect& rect, bool bAddToSelect);
    QVector3D getTerrainPos(bool bLand = true);
    QVector3D getTerrainPos(const int cursorPosX, const int cursorPosY, bool bLand = true);
    void changeOperation(EButtonOp type);
    void operationSetBackup(EOperationAxisType operationType);
    void operationRevert(EOperationAxisType operationType);
    void operationApply(EOperationAxisType operationType);
    void moveTo(QVector3D& dir);
    void rotateTo(QVector3D& rot);
    void scaleTo(QVector3D& scale);
    void resetUnitLogicPaths();
    void deleteSelectedNodes();
    void selectedObjectToClipboardBuffer();
    void clipboradObjectsToScene();
    void hideSelectedNodes();
    void unHideAll();
    CMob* currentMob() {return m_activeMob;}
    QOpenGLShaderProgram& shaderObject() {return m_program;}
    void setDurty(CMob* pMob = nullptr);
    void resetCamPosition();
    void addLogicPoint(bool bLookPoint = false);
    void copySelectedIDsToClipboard();
    void changeCurrentMob(CMob* pMob);
    void changeCurrentMob(QString mobName);
    void roundActiveMob();
    void undo_roundActiveMob();
    void execUnloadCommand();
    void iterateRoundMob();
    void applyRoundMob();
    void saveRecent();
    void openRecent();
    bool isRecentAvailable();
    int renameActiveMobUnits(QMap<QString, QString>& mapName);
    void moveCamToSelectedObjects();
    CTreeView* objectTree() {return m_pTree;}
    int nSelectedNodes();
    QList<CNode*> selectedNodes();
    void setRandomComplection(const EObjParam param, const float min, const float max);
    void resetSelectedId();
    void openMapParameters();
    void pickTile(QVector3D posOnLand, bool bLand = true);
    void setTile(QVector3D posOnLand, bool bLand = true);
    void updatePreviewTile(QVector3D posOnLand, bool bLand = true);
    void addTileRotation(int step);
    void setDrawWater(bool bDraw = true) {m_bDrawWater = bDraw;}
    void setDrawLand(bool bDraw = true) {m_bDrawLand = bDraw;}
    bool isPreviewTile() const {return m_bPreviewTile;}
    void setPreviewTile(bool bDraw = true) {m_bPreviewTile = bDraw;}
    void showOutliner(bool bShow = true);
    void pickQuickAccessTile(int index);

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int width, int height) override;
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;

private:
    void initShaders();
    void draw();
    CNode* pickObject(QList<CNode*>& aNode, int x, int y);
    void applyParam(SParam& param);
    void getColorFromRect(const QRect& rect, QVector<SColor>& aColor);
    void onParamChangeLogic(CNode* pNode, const QSharedPointer<IPropertyBase>& prop);
    void logOpenGlData();
    void checkOpenGlError();
    void drawTilePreview(QOpenGLShaderProgram* program);
    void updateTileForm();

public slots:
    void updateWindow();
    void updateParameter(EObjParam param);
    void viewParameters();
    void updateReadState(EReadState state); //get signal from reading texture/objects/map/mob
    void onParamChange(const QSharedPointer<IPropertyBase>& prop);
    void collectObjectTreeData();
    void onRestoreCursor();
    void onChangeCursorTile(QPixmap ico);
    void onMapMaterialUpdate();

    void execMobSwitch();
    void clearHistory();
    void onMobParamEditFinished(CMobParameters* pMob);
    void checkNewLandVersion();

signals:
    void updateMsg(QString msg);
    void unloadMob(CMob*);
    void updateMainWindowTitle(eTitleTypeData, QString);
    void showOutlinerSignal(bool bShow);

private:
    int m_height; //for redraw window
    int m_width;
    QSharedPointer<CCamera> m_cam;
    QOpenGLShaderProgram m_program;
    QOpenGLShaderProgram m_landProgram;
    QOpenGLShaderProgram m_selectProgram;
    QMatrix4x4 m_projection;
    QVector<CMob*> m_aMob;
    QTimer* m_timer;
    CSettings* m_pSettings;
    QVector<bool> m_aReadState;
    QSharedPointer<CTableManager> m_tableManager;
    QUndoStack* m_pUndoStack;
    CProgressView* m_pProgress;
    QSharedPointer<COperation> m_pOp;
    QMap<CNode*, QVector3D> m_operationBackup;
    //EOperationType m_operationType;
    QSharedPointer<CSelectFrame> m_selectFrame;
    QFile m_clipboard_buffer_file;
    QFile m_recentOpenedFile_file;
    CMob* m_activeMob;
    CTreeView* m_pTree;
    QList<CMobParameters*> m_arrParamWindow;
    CRoundMobForm* m_pRoundForm;
    QTimer* m_mprModifyTimer;
    QDateTime m_lastModifiedLand;
    bool m_bDrawLand;
    bool m_bDrawWater;
    bool m_bPreviewTile;
    CTileForm* m_pTileForm;
    QSharedPointer<CPreviewTile> m_pPreviewTile;
    CLandscape* m_pLand;
};

#endif // MYGLWIDGET_H
