#ifndef MATH_UTILS_H
#define MATH_UTILS_H
#include <QVector3D>

namespace util
{
    bool pointIsInTriangle();
    bool ptToTriangle(float& t, float& u, float& v, QVector3D& origin, QVector3D& dir, QVector3D& vert0, QVector3D& vert1, QVector3D& vert2, bool bTestCull = false);
}


#endif // MATH_UTILS_H
