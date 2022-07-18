#ifndef PREVIEW_H
#define PREVIEW_H

#include <QGLWidget>
#include <QMatrix4x4>
#include <QOpenGLShaderProgram>
#include "types.h"

class CNode;
class CView;
class CCamera;
class CSettings;

class CPreview : public QGLWidget
{
    Q_OBJECT

public:
    explicit CPreview(QWidget* parent=nullptr);
    //explicit CPreview(QWidget* parent=nullptr, const QGLWidget* pShareWidget=nullptr);
    void attachView(CView* pView) {m_pView = pView;}
    void attachNode(CNode* pNode);
    void attachSettings(CSettings* pSet);
    ~CPreview();

protected:
    void initializeGL() override;
    void resizeGL(int width, int height) override;
    void paintGL() override;
//    void keyPressEvent(QKeyEvent* event) override;
//    void keyReleaseEvent(QKeyEvent* event) override;
//    void mousePressEvent(QMouseEvent* event) override;
//    void mouseMoveEvent(QMouseEvent* event) override;
//    void mouseReleaseEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

private:
    void draw();

private slots:
    void refreshCam(CBox nodePos);

private:
    CNode* m_pNode;
    int m_height;
    int m_width;
    QMatrix4x4 m_projection;
    CView* m_pView;
    QSharedPointer<CCamera> m_cam;
};


#endif // PREVIEW_H
