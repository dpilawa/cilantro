#ifndef _PRIMITIVES_H_
#define _PRIMITIVES_H_

#include "cilantroengine.h"
#include "scene/MeshObject.h"

class Primitives
{
public:
    __EAPI static void GeneratePlane (MeshObject& m);
    __EAPI static void GenerateCube (MeshObject& m);
    __EAPI static void GenerateSphere (MeshObject& m, unsigned int subdivisions);
    __EAPI static void GenerateCone (MeshObject& m, unsigned int subdivisions);
    __EAPI static void GenerateCylinder (MeshObject& m, unsigned int subdivisions);
private:
    static void GenerateMeshFromArrays (MeshObject& m, const std::vector<Vector3f>& vertices, const std::vector<Vector2f>& uv, const std::vector<std::size_t> varray);
};

#endif