#ifndef _AABB_H_
#define _AABB_H_

#include "cilantroengine.h"
#include "math/Vector3f.h"
#include "math/Vector4f.h"
#include "math/Matrix4f.h"
#include "resource/Mesh.h"


namespace cilantro {

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

    // calculate for Mesh
    __EAPI void CalculateForMesh (std::shared_ptr<Mesh> mesh, const Matrix4f& worldTransform);

    // get eight vertices of AABB
    __EAPI std::array<Vector3f, 8> GetVertices () const;
    __EAPI float* GetVerticesData ();
    __EAPI uint32_t* GetIndicesData ();

private:
    Vector3f m_lowerBound; // lower bound of AABB
    Vector3f m_upperBound; // upper bound of AABB

    std::vector<float> m_vertices;
    std::vector<uint32_t> m_indices = { 0, 1, 2, 3, 2, 0, 3, 1,
                                        4, 5, 6, 7, 4, 6, 7, 5,
                                        2, 6, 3, 7, 0, 4, 1, 5 };
};

__EAPI AABB operator+ (AABB u, const AABB& v);

} // namespace cilantro

#endif

