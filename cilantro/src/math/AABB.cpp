#include "math/AABB.h"
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
    size_t count = mesh->GetVertexCount ();

    for (size_t i = 0; i < count; ++i)
    {
        Vector3f vertex (data[i * 3], data[i * 3 + 1], data[i * 3 + 2]);
        Vector4f worldVertex = worldTransform * Vector4f (vertex, 1.0f);
        AddVertex (worldVertex);
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

uint32_t* AABB::GetIndicesData ()
{
    return m_indices.data ();
}

AABB operator+ (AABB u, const AABB& v)
{
    u += v;
    return u;
}

}