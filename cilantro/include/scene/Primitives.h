#ifndef _PRIMITIVES_H_
#define _PRIMITIVES_H_

#include "cilantroengine.h"
#include "scene/MeshObject.h"

class Primitives
{
public:
    __EAPI static void GenerateCube (MeshObject& m);
    __EAPI static void GenerateSphere (MeshObject& m, unsigned int subdivisions, bool sharedVertices);
};

#endif