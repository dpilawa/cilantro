#include "TestHelpers.h"
#include "math/AABB.h"
#include <limits>
#include <algorithm>
#include <cstdint>
#include <set>
#include <tuple>

using namespace cilantro;
using namespace cilantro::testing;

namespace {

const float kPi = Mathf::Pi ();

AABB UnitBox ()
{
    return AABB (Vector3f (0.0f, 0.0f, 0.0f), Vector3f (1.0f, 2.0f, 3.0f));
}

} // namespace

TEST (AABB, DefaultIsEmptyWithInvertedInfiniteBounds)
{
    AABB box;
    const float inf = std::numeric_limits<float>::infinity ();

    EXPECT_EQ (box.GetLowerBound ()[0], inf);
    EXPECT_EQ (box.GetLowerBound ()[1], inf);
    EXPECT_EQ (box.GetLowerBound ()[2], inf);
    EXPECT_EQ (box.GetUpperBound ()[0], -inf);
    EXPECT_EQ (box.GetUpperBound ()[1], -inf);
    EXPECT_EQ (box.GetUpperBound ()[2], -inf);
}

TEST (AABB, ConstructsFromBounds)
{
    AABB box = UnitBox ();

    EXPECT_VEC3_NEAR (box.GetLowerBound (), Vector3f (0, 0, 0));
    EXPECT_VEC3_NEAR (box.GetUpperBound (), Vector3f (1, 2, 3));
}

TEST (AABB, AddVertexGrowsBoxToContainPoints)
{
    AABB box;

    box.AddVertex (Vector3f (1.0f, 2.0f, 3.0f));
    EXPECT_VEC3_NEAR (box.GetLowerBound (), Vector3f (1, 2, 3));
    EXPECT_VEC3_NEAR (box.GetUpperBound (), Vector3f (1, 2, 3));

    box.AddVertex (Vector3f (-1.0f, 5.0f, 0.0f));
    EXPECT_VEC3_NEAR (box.GetLowerBound (), Vector3f (-1, 2, 0));
    EXPECT_VEC3_NEAR (box.GetUpperBound (), Vector3f (1, 5, 3));

    // vertex inside does not change anything
    box.AddVertex (Vector3f (0.0f, 3.0f, 1.0f));
    EXPECT_VEC3_NEAR (box.GetLowerBound (), Vector3f (-1, 2, 0));
    EXPECT_VEC3_NEAR (box.GetUpperBound (), Vector3f (1, 5, 3));
}

TEST (AABB, UnionOfBoxes)
{
    AABB a (Vector3f (0, 0, 0), Vector3f (1, 1, 1));
    AABB b (Vector3f (-1, 0.5f, 0.5f), Vector3f (0.5f, 3, 0.75f));

    AABB sum = a + b;

    EXPECT_VEC3_NEAR (sum.GetLowerBound (), Vector3f (-1, 0, 0));
    EXPECT_VEC3_NEAR (sum.GetUpperBound (), Vector3f (1, 3, 1));

    a += b;
    EXPECT_VEC3_NEAR (a.GetLowerBound (), sum.GetLowerBound ());
    EXPECT_VEC3_NEAR (a.GetUpperBound (), sum.GetUpperBound ());
}

TEST (AABB, UnionWithEmptyBoxIsNeutral)
{
    AABB box = UnitBox ();
    AABB sum = box + AABB ();

    EXPECT_VEC3_NEAR (sum.GetLowerBound (), box.GetLowerBound ());
    EXPECT_VEC3_NEAR (sum.GetUpperBound (), box.GetUpperBound ());

    AABB reverse = AABB () + box;
    EXPECT_VEC3_NEAR (reverse.GetLowerBound (), box.GetLowerBound ());
    EXPECT_VEC3_NEAR (reverse.GetUpperBound (), box.GetUpperBound ());
}

TEST (AABB, CopyMoveAndAssignmentKeepBounds)
{
    AABB a = UnitBox ();
    AABB copy (a);
    AABB moved (std::move (copy));
    AABB assigned;
    assigned = a;

    EXPECT_VEC3_NEAR (moved.GetUpperBound (), a.GetUpperBound ());
    EXPECT_VEC3_NEAR (assigned.GetLowerBound (), a.GetLowerBound ());
    EXPECT_VEC3_NEAR (assigned.GetUpperBound (), a.GetUpperBound ());
}

TEST (AABB, VerticesAreTheEightCorners)
{
    AABB box = UnitBox ();
    auto vertices = box.GetVertices ();

    std::set<std::tuple<float, float, float>> corners;
    for (const auto& v : vertices)
    {
        // every coordinate is either the lower or the upper bound
        EXPECT_TRUE (v[0] == 0.0f || v[0] == 1.0f);
        EXPECT_TRUE (v[1] == 0.0f || v[1] == 2.0f);
        EXPECT_TRUE (v[2] == 0.0f || v[2] == 3.0f);
        corners.insert ({ v[0], v[1], v[2] });
    }

    EXPECT_EQ (corners.size (), 8u);
    EXPECT_VEC3_NEAR (vertices[0], box.GetLowerBound ());
    EXPECT_VEC3_NEAR (vertices[7], box.GetUpperBound ());
}

TEST (AABB, VerticesDataIsFlattenedVertices)
{
    AABB box = UnitBox ();
    auto vertices = box.GetVertices ();
    float* data = box.GetVerticesData ();

    for (unsigned int i = 0; i < 8; i++)
    {
        EXPECT_FLOAT_EQ (data[i * 3 + 0], vertices[i][0]);
        EXPECT_FLOAT_EQ (data[i * 3 + 1], vertices[i][1]);
        EXPECT_FLOAT_EQ (data[i * 3 + 2], vertices[i][2]);
    }
}

TEST (AABB, LineIndicesDescribeTwelveDistinctEdgesBetweenCorners)
{
    AABB box = UnitBox ();
    uint32_t* indices = box.GetLineIndicesData ();
    auto vertices = box.GetVertices ();

    std::set<std::pair<uint32_t, uint32_t>> edges;
    for (unsigned int e = 0; e < 12; e++)
    {
        uint32_t a = indices[e * 2];
        uint32_t b = indices[e * 2 + 1];

        ASSERT_LT (a, 8u);
        ASSERT_LT (b, 8u);
        EXPECT_NE (a, b);

        // an edge connects corners that differ in exactly one coordinate
        int differing = 0;
        for (unsigned int c = 0; c < 3; c++)
        {
            differing += (vertices[a][c] != vertices[b][c]) ? 1 : 0;
        }
        EXPECT_EQ (differing, 1);

        edges.insert ({ std::min (a, b), std::max (a, b) });
    }

    EXPECT_EQ (edges.size (), 12u);
}

TEST (AABB, TriangleIndicesReferenceCorners)
{
    AABB box = UnitBox ();
    uint32_t* indices = box.GetTriangleIndicesData ();

    for (unsigned int i = 0; i < 36; i++)
    {
        EXPECT_LT (indices[i], 8u);
    }
}

TEST (AABB, TrianglesCoverSurfaceOfBox)
{
    AABB box = UnitBox ();
    auto triangles = box.GetTriangles ();

    float area = 0.0f;
    for (const auto& t : triangles)
    {
        area += 0.5f * Mathf::Length (Mathf::Cross (t[1] - t[0], t[2] - t[0]));
    }

    // box 1 x 2 x 3: 2 * (1*2 + 1*3 + 2*3) = 22
    EXPECT_NEAR (area, 22.0f, 1e-3f);
}

TEST (AABBTransform, IdentityKeepsBounds)
{
    AABB box = UnitBox ();
    AABB transformed = box.ToSpace (Identity4 ());

    EXPECT_VEC3_NEAR (transformed.GetLowerBound (), box.GetLowerBound ());
    EXPECT_VEC3_NEAR (transformed.GetUpperBound (), box.GetUpperBound ());
}

TEST (AABBTransform, TranslationShiftsBounds)
{
    AABB transformed = UnitBox ().ToSpace (Mathf::GenTranslationMatrix (10.0f, -1.0f, 0.5f));

    EXPECT_VEC3_NEAR (transformed.GetLowerBound (), Vector3f (10.0f, -1.0f, 0.5f));
    EXPECT_VEC3_NEAR (transformed.GetUpperBound (), Vector3f (11.0f, 1.0f, 3.5f));
}

TEST (AABBTransform, ScalingScalesBounds)
{
    AABB transformed = UnitBox ().ToSpace (Mathf::GenScalingMatrix (2.0f, 0.5f, 1.0f));

    EXPECT_VEC3_NEAR (transformed.GetLowerBound (), Vector3f (0.0f, 0.0f, 0.0f));
    EXPECT_VEC3_NEAR (transformed.GetUpperBound (), Vector3f (2.0f, 1.0f, 3.0f));
}

TEST (AABBTransform, QuarterTurnAroundZSwapsAndMirrorsExtents)
{
    // x' = -y, y' = x
    AABB transformed = UnitBox ().ToSpace (Mathf::GenRotationZMatrix (kPi * 0.5f));

    EXPECT_VEC3_NEAR (transformed.GetLowerBound (), Vector3f (-2.0f, 0.0f, 0.0f));
    EXPECT_VEC3_NEAR (transformed.GetUpperBound (), Vector3f (0.0f, 1.0f, 3.0f));
}

TEST (AABBTransform, ArbitraryRotationProducesBoxContainingRotatedCorners)
{
    AABB box = UnitBox ();
    Matrix4f rotation = Mathf::GenRotationXYZMatrix (0.4f, 0.8f, -0.3f);
    AABB transformed = box.ToSpace (rotation);

    for (const auto& corner : box.GetVertices ())
    {
        Vector3f p (rotation * Vector4f (corner, 1.0f));

        for (unsigned int c = 0; c < 3; c++)
        {
            EXPECT_GE (p[c], transformed.GetLowerBound ()[c] - 1e-4f);
            EXPECT_LE (p[c], transformed.GetUpperBound ()[c] + 1e-4f);
        }
    }
}
