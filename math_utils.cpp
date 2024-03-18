#include "math_utils.h"

namespace util
{

///
/// \brief checks if the point belongs to the triangle
/// \param in. pos - point
/// \param in. pt1 - first point of triagle
/// \param in. pt2 - second point of triangle
/// \param in. pt3 - third point of triangle
/// \return
///
bool pointIsInTriangle(QVector3D& pos, QVector3D& pt1, QVector3D& pt2, QVector3D& pt3)
{
    float a = (pt1.x() - pos.x()) * (pt2.y() - pt1.y()) - (pt2.x() - pt1.x()) * (pt1.y() - pos.y());
    float b = (pt2.x() - pos.x()) * (pt3.y() - pt2.y()) - (pt3.x() - pt2.x()) * (pt2.y() - pos.y());
    float c = (pt3.x() - pos.x()) * (pt1.y() - pt3.y()) - (pt1.x() - pt3.x()) * (pt3.y() - pos.y());

    return ((a >= 0 && b >= 0 && c >= 0) || (a <= 0 && b <= 0 && c <= 0));
}


///
/// \brief calculates the distance from a point to a triangle
/// \param out. t - projected coordinate
/// \param out. u - projected coordinate
/// \param out. v - projected coordinate
/// \param i. origin - the point from which the distance is to be calculated
/// \param in. dir - direction from the point to the triangle
/// \param in. vert0 - first point of triagle
/// \param in. vert1 - second point of triangle
/// \param in. vert2 - third point of triangle
/// \param in. bTestCull
/// \return true if it has an intersection with a triangle
///
bool ptToTriangle(float& t, float& u, float& v, QVector3D& origin, QVector3D& dir, QVector3D& vert0, QVector3D& vert1, QVector3D& vert2, bool bTestCull)
{
    QVector3D edge1 = vert1-vert0;
    QVector3D edge2 = vert2-vert0;
    QVector3D pvec = QVector3D::crossProduct(dir, edge2);
    float det = QVector3D::dotProduct(edge1, pvec);
    const float epsilon = 0.00001f;
    if(bTestCull)
    {
        if (det < epsilon)
            return false;

        QVector3D tvec = origin - vert0;
        u = QVector3D::dotProduct(tvec, pvec);
        if (u < 0.0f || u > det)
            return false;

        QVector3D qvec = QVector3D::crossProduct(tvec, edge1);
        v = QVector3D::dotProduct(dir, qvec);
        if (v < 0.0f || v + u > det)
            return false;

        t = QVector3D::dotProduct(edge2, qvec);
        float invDet = 1.0f / det;
        u *= invDet;
        t *= invDet;
        v *= invDet;
    }
    else
    {
        if ((det > epsilon*(-1)) && (det < epsilon))
            return false;

        const float invDet = 1.0f / det;
        QVector3D tvec = origin - vert0;
        u = QVector3D::dotProduct(tvec, pvec) * invDet;
        if (u < 0.0f || u > 1.0f)
            return false;

        QVector3D qvec = QVector3D::crossProduct(tvec, edge1);
        v = QVector3D::dotProduct(dir, qvec) * invDet;
        if (v < 0.0f || u + v > 1.0f)
            return false;

        t = QVector3D::dotProduct(edge2, qvec) * invDet;
    }
    return true;
}

}
