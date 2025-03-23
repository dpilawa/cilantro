#ifndef _PRIMITIVES_H_
#define _PRIMITIVES_H_

#include "cilantroengine.h"
#include <vector>
#include <memory>

namespace cilantro {

class Mesh;
class Vector2f;
class Vector3f;

class __CEAPI Primitives
{
public:
    static __EAPI void GeneratePlane (std::shared_ptr<Mesh> m);
    static __EAPI void GenerateCube (std::shared_ptr<Mesh> m);
    static __EAPI void GenerateSphere (std::shared_ptr<Mesh> m, unsigned int subdivisions);
    static __EAPI void GenerateCone (std::shared_ptr<Mesh> m, unsigned int subdivisions);
    static __EAPI void GenerateCylinder (std::shared_ptr<Mesh> m, unsigned int subdivisions);
private:
    static void GenerateMeshFromArrays (std::shared_ptr<Mesh> m, const std::vector<Vector3f>& vertices, const std::vector<Vector2f>& uv, const std::vector<std::size_t> varray);
};

} // namespace cilantro

#endif