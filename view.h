#ifndef MYGLWIDGET_H
#define MYGLWIDGET_H

#include <QGLWidget>
#include <QOpenGLShader>
#include <QFileInfo>
#include <QTimer>
#include <QSharedPointer>

class CObjectList;
class CTextureList;
class CKeyManager;
class CMob;
class CCamera;
class CNode;
class CLandscape;
class CKeyManager;

class CView : public QGLWidget
{
    Q_OBJECT

public:
    explicit CView(QWidget* parent=nullptr);
    ~CView();

    void loadLandscape(QFileInfo& filePath);
    void loadMob(QFileInfo& filePath);
    CLandscape* land() {Q_ASSERT(m_landscape); return m_landscape;}
    bool isLandLoaded() {return nullptr != m_landscape;}
    CTextureList* texList();
    CObjectList* objList();

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
    void projectToLandscape(QList<CNode*>& aNode);
    void unloadMob();
    void unloadLand();

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


public slots:
    void updateWindow();
    //sliders for moving\rotating objects(value come from slider)
//    void setXRot(int angle);
//    void setYRot(int angle);
//    void setZRot(int angle);
//    void setXOffset(int offset);
//    void setYOffset(int offset);
//    void setZOffset(int offset);

signals:
    //set signal to change slider value
//    void xRotationChanged(int angle);
//    void yRotationChanged(int angle);
//    void zRotationChanged(int angle);
};

#endif // MYGLWIDGET_H
