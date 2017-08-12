#ifndef FIGURE_H
#define FIGURE_H
#include <QVector>

#include <QString>
#include <QDebug>

class figure
{
public:
    figure();
    void show();

private:
    QString name;
    QVector <int> header;
    //QVector <float> center;
    //QVector <float> min;
    //QVector <float> max;
    //QVector <float> radius;
    //QVector <float> verts;
    //QVector <float> normals;
    //QVector <float> t_coords;
    QVector <float> indices;
    //QVector <float> light_comp;

};

#endif // FIGURE_H
