#ifndef EI_TYPES_H
#define EI_TYPES_H

#include "QVector"
#include "QString"
#include "QList"
#include "QDebug"
#include <fstream>

namespace ei {

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

struct vec3
{
    float x;
    float y;
    float z;
};

struct vec4
{
    float x;
    float y;
    float z;
    float w;
};

}


#endif
