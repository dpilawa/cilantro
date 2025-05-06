#ifndef _AABB_H_
#define _AABB_H_

#include "cilantroengine.h"
#include "math/Vector3f.h"
#include "math/Vector4f.h"
#include "math/Matrix4f.h"
#include "math/Triangle.h"
#include <vector>
#include <memory>
#include <array>

namespace cilantro {

class Mesh;
class MeshObject;

// Axis-aligned bounding box (AABB)
class __CEAPI AABB
{
public:
    // constructors
    __EAPI AABB ();
    
    // copy constructor
    __EAPI AABB (const AABB& other);

    // move constructor
    __EAPI AABB (AABB&& other);

    // assignment operator
    __EAPI AABB& operator= (AABB other);

    // destructor
    __EAPI ~AABB ();

    // union of two AABBs
    __EAPI AABB& operator+= (const AABB& other);

    // add vertex to AABB
    __EAPI void AddVertex (const Vector3f& v);

    // calculate
    __EAPI void CalculateForMesh (std::shared_ptr<Mesh> mesh, const Matrix4f& worldTransform);
    __EAPI void CalculateForMeshObject (std::shared_ptr<MeshObject> meshObject);

    // get eight vertices of AABB
    __EAPI std::array<Vector3f, 8> GetVertices () const;
    __EAPI float* GetVerticesData ();
    __EAPI uint32_t* GetLineIndicesData ();
    __EAPI uint32_t* GetTriangleIndicesData ();

    // get trinagle array in given space
    __EAPI std::array<Triangle<Vector3f>, 12> GetTriangles (const Matrix4f& spaceTransform) const;

private:
    Vector3f m_lowerBound; // lower bound of AABB
    Vector3f m_upperBound; // upper bound of AABB

    std::vector<float> m_vertices;

    // lines
    std::vector<uint32_t> m_lineIndices = { 0, 1,  2, 3,  2, 0,  3, 1,
                                          4, 5,  6, 7,  4, 6,  7, 5,
                                          2, 6,  3, 7,  0, 4,  1, 5 };
    // triangles
    std::vector<uint32_t> m_triangleIndices = { 0, 1, 2,  2, 3, 0,
                                             4, 5, 6,  6, 7, 4,
                                             0, 1, 4,  4, 5, 0,
                                             2, 3, 6,  6, 7, 2,
                                             0, 2, 4,  4, 6, 0,
                                             1, 3, 5,  5, 7, 1 };

};

__EAPI AABB operator+ (AABB u, const AABB& v);

} // namespace cilantro

#endif

