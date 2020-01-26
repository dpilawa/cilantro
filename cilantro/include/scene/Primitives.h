#ifndef _PRIMITIVES_H_
#define _PRIMITIVES_H_

#include "cilantroengine.h"
#include "scene/MeshObject.h"

class Primitives
{
public:
    __EAPI static void GeneratePlane (MeshObject& m, bool sharedVertices);
    __EAPI static void GenerateCube (MeshObject& m, bool sharedVertices);
    __EAPI static void GenerateSphere (MeshObject& m, unsigned int subdivisions, bool sharedVertices);
private:
    static void GenerateMeshFromArrays (MeshObject& m, bool sharedVertices, const std::vector<Vector3f>& vertices, const std::vector<std::size_t> array);
};

#endif