#ifndef FIGURE_H
#define FIGURE_H
#include "ei_types.h"

namespace ei {
class CFigure{
public:
    CFigure();
    ~CFigure();
    QVector<f3>& vertices();
    void normals();
    void uvCoords();
    void boundBox();
    void setComplex(float str, float dex, float tall);
    //TODO return methods of vertices, uv, normals, vert.indices, uv.indices
    bool loadFromFile(const wchar_t* path);
    void calculateConstitution (f3 constitute);
    void draw();
private:
    //TODO: change vector to array
    QVector<float> m_BoundBox;

    QVector<f3> m_vertices;
    QVector<int> m_vertIndices;
    QVector<f4> m_normals;
    QVector<int> m_normIndices;
    QVector<f2> m_uvCoords;
    QVector<int> m_uvIndices;

    // vectors include morph components
    QVector<QVector<f3>> m_morphVertices;
    //TODO: change vector to array or bbox class
    QVector <f3> m_morphMin;   // 8x3
    QVector <f3> m_morphMax;   // 8x3
    QVector <f3> m_morphCenter; //8x3
    QVector <float> m_morphRadius;  //8
};
}

#endif // FIGURE_H
