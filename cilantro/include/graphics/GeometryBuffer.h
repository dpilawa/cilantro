#ifndef _GEOMETRYBUFFER_H_
#define _GEOMETRYBUFFER_H_

#include "cilantroengine.h"

#define CILANTRO_VBO_COUNT 7

enum VBOType { VBO_VERTICES = 0, VBO_NORMALS, VBO_UVS, VBO_TANGENTS, VBO_BITANGENTS, VBO_BONES, VBO_BONEWEIGHTS };

class GeometryBuffer
{
public:
    unsigned int vertexCount = 0;
};

#endif