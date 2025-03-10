#ifndef _PRIMITIVES_H_
#define _PRIMITIVES_H_

#include "cilantroengine.h"
#include <vector>

namespace cilantro {

class Mesh;
class Vector2f;
class Vector3f;

class Primitives
{
public:
    __EAPI static void GeneratePlane (Mesh& m);
    __EAPI static void GenerateCube (Mesh& m);
    __EAPI static void GenerateSphere (Mesh& m, unsigned int subdivisions);
    __EAPI static void GenerateCone (Mesh& m, unsigned int subdivisions);
    __EAPI static void GenerateCylinder (Mesh& m, unsigned int subdivisions);
private:
    static void GenerateMeshFromArrays (Mesh& m, const std::vector<Vector3f>& vertices, const std::vector<Vector2f>& uv, const std::vector<std::size_t> varray);
};

} // namespace cilantro

#endif