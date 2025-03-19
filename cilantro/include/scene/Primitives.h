#ifndef _PRIMITIVES_H_
#define _PRIMITIVES_H_

#include "cilantroengine.h"
#include <vector>

namespace cilantro {

class Mesh;
class Vector2f;
class Vector3f;

class __CEAPI Primitives
{
public:
    static __EAPI void GeneratePlane (Mesh& m);
    static __EAPI void GenerateCube (Mesh& m);
    static __EAPI void GenerateSphere (Mesh& m, unsigned int subdivisions);
    static __EAPI void GenerateCone (Mesh& m, unsigned int subdivisions);
    static __EAPI void GenerateCylinder (Mesh& m, unsigned int subdivisions);
private:
    static void GenerateMeshFromArrays (Mesh& m, const std::vector<Vector3f>& vertices, const std::vector<Vector2f>& uv, const std::vector<std::size_t> varray);
};

} // namespace cilantro

#endif