#include "math/AABB.h"
#include "scene/MeshObject.h"
#include "resource/Mesh.h"
#include <limits>
#include <array>

namespace cilantro {

AABB::AABB ()
{
    float posInf = std::numeric_limits<float>::infinity();
    float negInf = -std::numeric_limits<float>::infinity();

    m_lowerBound = Vector3f (posInf, posInf, posInf);
    m_upperBound = Vector3f (negInf, negInf, negInf);
}

AABB::AABB (const Vector3f& lowerBound, const Vector3f& upperBound)
{
    m_lowerBound = lowerBound;
    m_upperBound = upperBound;
}

// copy constructor
AABB::AABB (const AABB& other)
{
    m_lowerBound = other.m_lowerBound;
    m_upperBound = other.m_upperBound;
}

// move constructor
AABB::AABB (AABB&& other)
{
    m_lowerBound = std::move (other.m_lowerBound);
    m_upperBound = std::move (other.m_upperBound);
}

// assignment operator
AABB& AABB::operator= (AABB other)
{
    std::swap (m_lowerBound, other.m_lowerBound);
    std::swap (m_upperBound, other.m_upperBound);
    return *this;
}

AABB::~AABB ()
{
}

AABB& AABB::operator+=(const AABB& other)
{
    m_lowerBound[0] = std::min (m_lowerBound[0], other.m_lowerBound[0]);
    m_lowerBound[1] = std::min (m_lowerBound[1], other.m_lowerBound[1]);
    m_lowerBound[2] = std::min (m_lowerBound[2], other.m_lowerBound[2]);

    m_upperBound[0] = std::max (m_upperBound[0], other.m_upperBound[0]);
    m_upperBound[1] = std::max (m_upperBound[1], other.m_upperBound[1]);
    m_upperBound[2] = std::max (m_upperBound[2], other.m_upperBound[2]);

    return *this;
}

void AABB::AddVertex (const Vector3f& v)
{
    m_lowerBound[0] = std::min (m_lowerBound[0], v[0]);
    m_lowerBound[1] = std::min (m_lowerBound[1], v[1]);
    m_lowerBound[2] = std::min (m_lowerBound[2], v[2]);

    m_upperBound[0] = std::max (m_upperBound[0], v[0]);
    m_upperBound[1] = std::max (m_upperBound[1], v[1]);
    m_upperBound[2] = std::max (m_upperBound[2], v[2]);
}

void AABB::CalculateForMesh (std::shared_ptr<Mesh> mesh, const Matrix4f& worldTransform)
{
    m_lowerBound = Vector3f (std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity());
    m_upperBound = Vector3f (-std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity());

    float* data = mesh->GetVerticesData ();

    // claculate AABB for each vertex in world space
    for (size_t v = 0; v < mesh->GetVertexCount (); ++v)
    {
        Vector3f vertex (data[v * 3], data[v * 3 + 1], data[v * 3 + 2]);
        Vector4f worldVertex = worldTransform * Vector4f (vertex, 1.0f);
        AddVertex (worldVertex);
    }
}

void AABB::CalculateForMeshObject (std::shared_ptr<MeshObject> meshObject)
{
    m_lowerBound = Vector3f (std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity());
    m_upperBound = Vector3f (-std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity());

    auto mesh = meshObject->GetMesh ();
    float* data = mesh->GetVerticesData ();
    auto worldTransform = meshObject->GetWorldTransformMatrix ();

    // apply bone transformations
    auto boneTransformations = meshObject->GetBoneTransformationsMatrixArray ();
    for (size_t v = 0; v < mesh->GetVertexCount (); v++)
    {
        Vector3f modelVertex (data[v * 3], data[v * 3 + 1], data[v * 3 + 2]);
        Vector4f worldVertex = worldTransform * Vector4f (modelVertex, 1.0f);
        Vector4f transformedVertex = Vector4f (0.0f, 0.0f, 0.0f, 0.0f);
        bool transformed = false;

        for (size_t i = 0; i < mesh->GetBoneInfluenceCounts ()[v]; i++)
        {
            size_t boneIndex = mesh->GetBoneIndicesData ()[v * CILANTRO_MAX_BONE_INFLUENCES + i];
            float boneWeight = mesh->GetBoneWeightsData ()[v * CILANTRO_MAX_BONE_INFLUENCES + i];
           
            transformedVertex += boneWeight * Matrix4f (boneTransformations + boneIndex * 16) * worldVertex;
            transformed = true;
        }

        transformed ? AddVertex (transformedVertex) : AddVertex (worldVertex);
    }
}

std::array<Vector3f, 8> AABB::GetVertices () const
{
    std::array<Vector3f, 8> vertices;

    vertices[0] = m_lowerBound;
    vertices[1] = Vector3f (m_lowerBound[0], m_lowerBound[1], m_upperBound[2]);
    vertices[2] = Vector3f (m_lowerBound[0], m_upperBound[1], m_lowerBound[2]);
    vertices[3] = Vector3f (m_lowerBound[0], m_upperBound[1], m_upperBound[2]);
    vertices[4] = Vector3f (m_upperBound[0], m_lowerBound[1], m_lowerBound[2]);
    vertices[5] = Vector3f (m_upperBound[0], m_lowerBound[1], m_upperBound[2]);
    vertices[6] = Vector3f (m_upperBound[0], m_upperBound[1], m_lowerBound[2]);
    vertices[7] = m_upperBound;

    return vertices;
}

float* AABB::GetVerticesData ()
{
    m_vertices.clear ();
    m_vertices.reserve (8 * 3);
    auto vertices = GetVertices ();
    for (const auto& vertex : vertices)
    {
        m_vertices.push_back (vertex[0]);
        m_vertices.push_back (vertex[1]);
        m_vertices.push_back (vertex[2]);
    }
    return m_vertices.data ();
}

uint32_t* AABB::GetLineIndicesData ()
{
    return m_lineIndices.data ();
}

uint32_t* AABB::GetTriangleIndicesData ()
{
    return m_triangleIndices.data ();
}   

std::array<Triangle<Vector3f>, 12> AABB::GetTriangles () const
{
    std::array<Triangle<Vector3f>, 12> triangles;
    auto aabbVertices = GetVertices ();
    
    // generate triangles
    for (unsigned int i = 0; i < 12; ++i)
    {
        Vector4f v1 = Vector4f (aabbVertices[m_triangleIndices[i * 3]], 1.0f);
        Vector4f v2 = Vector4f (aabbVertices[m_triangleIndices[i * 3 + 1]], 1.0f);
        Vector4f v3 = Vector4f (aabbVertices[m_triangleIndices[i * 3 + 2]], 1.0f);

        triangles[i] = Triangle<Vector3f> (v1 / v1[3], v2 / v2[3], v3 / v3[3]);
    }

    return triangles;
}

AABB AABB::ToSpace (const Matrix4f& spaceTransform) const
{
    float minx = std::numeric_limits<float>::infinity ();
    float miny = std::numeric_limits<float>::infinity ();
    float minz = std::numeric_limits<float>::infinity ();
    float maxx = -std::numeric_limits<float>::infinity ();
    float maxy = -std::numeric_limits<float>::infinity ();
    float maxz = -std::numeric_limits<float>::infinity ();

    auto aabbVertices = GetVertices ();

    // conveer aabb vertices to given space
    for (unsigned int i = 0; i < 8; ++i)
    {
        Vector4f v = spaceTransform * Vector4f (aabbVertices[i], 1.0f);
        aabbVertices[i] = v / v[3];
    }

    // calculate aabb bounds in new space
    for (unsigned int i = 0; i < 8; ++i)
    {
        minx = std::min (minx, aabbVertices[i][0]);
        miny = std::min (miny, aabbVertices[i][1]);
        minz = std::min (minz, aabbVertices[i][2]);
        maxx = std::max (maxx, aabbVertices[i][0]);
        maxy = std::max (maxy, aabbVertices[i][1]);
        maxz = std::max (maxz, aabbVertices[i][2]);
    }

    return AABB (Vector3f (minx, miny, minz), Vector3f (maxx, maxy, maxz));
}

AABB operator+ (AABB u, const AABB& v)
{
    u += v;
    return u;
}

}