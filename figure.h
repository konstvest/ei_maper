#ifndef FIGURE_H
#define FIGURE_H
#include "ei_types.h"
#include <QOpenGLBuffer>

namespace ei {

struct SHeader{
    int vertBlocks = 0;
    int normalBlocks = 0;
    int uvCount = 0;
    int indexCount = 0;
    int vertexComponentCount = 0;
    int morphingComponentCount = 0;
    int unknown = 0;
    int group = 0;
    int textureNumber = 0;

    void read(QDataStream& stream);
};

class CFigure : protected QOpenGLFunctions{
public:
    CFigure();
    ~CFigure();
    QVector<f3>& vertices();
    void normals();
    void uvCoords();
    void boundBox();
    void setComplex(float str, float dex, float tall);
    //TODO return methods of vertices, uv, normals, vert.indices, uv.indices
    //bool loadFromFile(const wchar_t* path);
    bool readData(QDataStream& stream);
    void calculateConstitution (f3 constitute);
    void draw(QOpenGLShaderProgram* shaders);
    void verticesDataToOpenGlBufers();
private:
    //TODO: change vector to array
    QVector<float> m_BoundBox;

    QVector<f3> m_vertices;
    QVector<f4> m_normals;
    QVector<f2> m_uvCoords;

    QVector<unsigned short> m_indices;  //change to more container, not short
    QVector<f3> m_vComponents;
    //QVector<int> m_normIndices;
    //QVector<int> m_uvIndices;

    // vectors include morph components
    QVector<QVector<f3>> m_morphVertices;
    //TODO: change vector to array or bbox class
    QVector <f3> m_morphMin;   // 8x3
    QVector <f3> m_morphMax;   // 8x3
    QVector <f3> m_morphCenter; //8x3
    QVector <float> m_morphRadius;  //8

    QOpenGLBuffer m_verticesBuf;
    QOpenGLBuffer m_indicesBuf;
};
}

#endif // FIGURE_H
