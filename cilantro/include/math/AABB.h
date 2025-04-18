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

private:
    Vector3f m_lowerBound; // lower bound of AABB
    Vector3f m_upperBound; // upper bound of AABB
};

__EAPI AABB operator+ (AABB u, const AABB& v);

} // namespace cilantro

#endif

