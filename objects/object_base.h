#ifndef OBJECT_H
#define OBJECT_H

#include <QVector>
#include <QGLWidget>
#include <QOpenGLFunctions>
//#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
//#include <QOpenGLTexture>
#include "node.h"
#include "part.h"

class CObjectBase : public CNode
{
public:
    CObjectBase();
    CObjectBase(CNode* node);
    ~CObjectBase() override;

    void draw(QOpenGLShaderProgram* program) override;
    void drawSelect(QOpenGLShaderProgram* program = nullptr) override;
    ENodeType nodeType() override {return ENodeType::eUnknown; }    //todo: delete, use this class as abstract type
    QString& modelName() override {return m_modelName; }
    QString textureName() override {return QString("default00.mmp");}
    void updateFigure(ei::CFigure* fig) override;
    void setModelName(QString name) {m_modelName = name;}
    QVector3D& minPosition() override { return m_minPoint; }
    void setTexture(QOpenGLTexture* texture) override;
    void serializeJson(QJsonObject& obj) override;

protected:
    void updateVisibility(QVector<QString>& aPart);

protected:
    QString m_modelName;
    QVector3D m_complection;    //x-grace, y-strength, z-tall

private:
    QVector<CPart*> m_aPart;
    QVector3D m_minPoint;   // min position of model bound boxes(units half body under ground)
    QOpenGLTexture* m_texture;
};

#endif // OBJECT_H
