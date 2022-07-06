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

class CMob;

class CObjectBase : public CNode
{
public:
    CObjectBase();
    CObjectBase(CNode* node);
    ~CObjectBase() override;

    void draw(QOpenGLShaderProgram* program) override;
    void drawSelect(QOpenGLShaderProgram* program = nullptr) override;
    ENodeType nodeType() override {return ENodeType::eBaseType; }    //todo: delete, use this class as abstract type
    QString& modelName() override {return m_modelName; }
    QString textureName() override {return QString("default0");}
    void updateFigure(ei::CFigure* fig) override;
    void loadFigure() override;
    void loadTexture() override;
    void setModelName(QString name) {m_modelName = name;}
    QVector3D& minPosition() override { return m_minPoint; }
    void setTexture(QOpenGLTexture* texture) override;
    void serializeJson(QJsonObject& obj) override;
    uint serialize(util::CMobParser& parser) override;
    void collectParams(QMap<EObjParam, QString>& aParam, ENodeType paramType) override;
    void applyParam(EObjParam param, const QString& value) override;
    QString getParam(EObjParam param) override;
    bool updatePos(QVector3D& pos) override;
    void attachMob(CMob* mob);
    CMob* mob() {return m_pMob;}
    void setRot(const QQuaternion& quat) override;
    const QVector3D& constitution() override final {return m_complection;}
    const QVector3D& complection() override {return m_complection;}
    void setConstitution(QVector3D& vec) override;
    QJsonObject toJson() override;

protected:
    void recalcFigure();
    void recalcMinPos();
    //void updateVisibility(QVector<QString>& aPart);
    void addParam(QMap<EObjParam, QString>& aParam, EObjParam param, QString str);

protected:
    QString m_modelName;
    QVector3D m_complection;    //x-grace, y-strength, z-tall
    QVector<QString> m_bodyParts; // for preparing vertex data
    CMob* m_pMob;

private:
    QVector<CPart*> m_aPart;
    QVector3D m_minPoint;   // min position of model bound boxes(units half body under ground)
    QOpenGLTexture* m_texture;
    ei::CFigure* m_pFigure;
};

#endif // OBJECT_H
