#ifndef EI_TYPES_H
#define EI_TYPES_H

#include "QVector"
#include "QString"
#include "QList"
#include "QDebug"
#include <fstream>
#include "ei_vectors.h"

struct indices_link
{
    short normal_ind;
    short vertex_ind;
    short texture_ind;
};

typedef ei::vector2<short> s2;
typedef ei::vector2<int> i2;
typedef ei::vector2<float> f2;
typedef ei::vector2<double> d2;

typedef ei::vector3<short> s3;
typedef ei::vector3<int> i3;
typedef ei::vector3<float> f3;
typedef ei::vector3<double> d3;

typedef ei::vector4<short> s4;
typedef ei::vector4<int> i4;
typedef ei::vector4<float> f4;
typedef ei::vector4<double> d4;

#endif
