#ifndef FIGURE_H
#define FIGURE_H

#include <QVector>
#include <QList>

#include <QString>
#include <QDebug>
#include <fstream>

struct indices_link{
    short normal_ind;
    short vertex_ind;
    short texture_ind;
};

struct vec2{
    float x;
    float y;
};

struct vec3{
    float x;
    float y;
    float z;
};

struct vec4{
    float x;
    float y;
    float z;
    float w;
};

class figure
{
public:
    figure();

    QString name;
    QVector <float> vertices;
    QVector <vec4> normals;
    QVector <vec2> t_coords;
    QVector <unsigned int> indices;
    void loadFromFile(QString pathFile);
    void recalcConstitution (float str, float dex, float scale);
    void recalcTextureCoordinates (QString type);
    void convertToGLIndices ();
private:
    QVector <vec3> tvertices;
    QVector <vec4> tnormals;

    QVector <int> header;
    QVector <vec3> center;
    QVector <vec3> min;
    QVector <vec3> max;
    QVector <float> radius;
    QVector <QVector <vec3>> preVertices;
    QVector <vec2> preT_coords;
    QVector <short> preIndices;
    QVector <indices_link> vert_comp;
    QVector <short> light_comp;
};

#endif // FIGURE_H
