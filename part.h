#ifndef PART_H
#define PART_H
#include <QString>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include "types.h"

class CPart
{
public:
    CPart();
    CPart(const CPart& part);
    ~CPart();
    QVector<SVertexData>& vertData() {return m_aVertData; }
    QString& name() {return m_name; }
    void setName(QString& name) {m_name = name; }
    void setVisible(bool bShow = true) {m_bShow = bShow;}
    void update(); //update shader buffers
    void draw(QOpenGLShaderProgram* program);
    void drawSelect(QOpenGLShaderProgram* program);

private:
    QOpenGLBuffer m_vertexBuf;
    QOpenGLBuffer m_indexBuf;

    QString m_name;
    QVector<SVertexData> m_aVertData;
    bool m_bShow;
};

#endif // PART_H
