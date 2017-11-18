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

struct vec2
{
    float x;
    float y;
};

typedef ei::vector3<short> s3;
typedef ei::vector3<int> i3;
typedef ei::vector3<float> f3;
typedef ei::vector3<double> d3;

struct vec4
{
    float x;
    float y;
    float z;
    float w;
};

#endif
