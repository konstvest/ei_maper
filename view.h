#ifndef MYGLWIDGET_H
#define MYGLWIDGET_H

#include <QGLWidget>
#include <QOpenGLShader>
#include <QFileInfo>
#include <QTimer>
#include <QSharedPointer>

#include "types.h"

class QTextEdit;
class CObjectList;
class CTextureList;
class CKeyManager;
class CMob;
class CCamera;
class CNode;
class CLandscape;
class CKeyManager;
class CLogger;
class CSettings;

class CView : public QGLWidget
{
    Q_OBJECT

public:
    explicit CView(QWidget* parent=nullptr);
    ~CView();

    void loadLandscape(QFileInfo& filePath);
    void loadMob(QFileInfo& filePath);
    void saveMob(QFileInfo& file);
    void serializeMob(QFileInfo& file);
    CLandscape* land() {Q_ASSERT(m_landscape); return m_landscape;}
    bool isLandLoaded() {return nullptr != m_landscape;}
    CTextureList* texList();
    CObjectList* objList();
    void attachLogWindow(QTextEdit* pTextEdit);
    void attachSettings(CSettings* pSettings);
    CSettings* settings() {Q_ASSERT(m_pSettings); return m_pSettings;}
    void log(const char* msg);

protected:
    void initializeGL();
    void initShaders();
    void paintGL();
    void resizeGL(int width, int height);
    void keyPressEvent(QKeyEvent* event);
    void keyReleaseEvent(QKeyEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void wheelEvent(QWheelEvent* event);

private:
    void draw();
    CNode* pickObject(QList<CNode*>& aNode, QList<CNode*>& aNodeSelected, int x, int y);
    void delNodes();
    void unloadMob();
    void unloadLand();
    bool isResourceInitiated();

private:
    QPoint m_lastPos; // for mouse action
    int m_height;

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
    QSharedPointer<CKeyManager> m_keyManager;
    QSharedPointer<CLogger> m_logger;
    CSettings* m_pSettings;
    QVector<bool> m_aReadState;

public slots:
    void updateWindow();
    void updateReadState(EReadState state); //get signal from reading texture/objects/map/mob
    //sliders for moving\rotating objects(value come from slider)
//    void setXRot(int angle);
//    void setYRot(int angle);
//    void setZRot(int angle);
//    void setXOffset(int offset);
//    void setYOffset(int offset);
//    void setZOffset(int offset);

signals:
    void updateMsg(QString msg);

    //set signal to change slider value
//    void xRotationChanged(int angle);
//    void yRotationChanged(int angle);
//    void zRotationChanged(int angle);
};

#endif // MYGLWIDGET_H
