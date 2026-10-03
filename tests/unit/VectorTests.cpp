#include "TestHelpers.h"

using namespace cilantro;

// ---------------------------------------------------------------------------
// Vector2f
// ---------------------------------------------------------------------------

TEST (Vector2f, DefaultConstructsToZero)
{
    Vector2f v;

    EXPECT_FLOAT_EQ (v[0], 0.0f);
    EXPECT_FLOAT_EQ (v[1], 0.0f);
    EXPECT_EQ (v.Dim (), 2u);
}

TEST (Vector2f, ConstructsFromComponentsAndInitializerList)
{
    Vector2f a (1.0f, 2.0f);
    Vector2f b { 3.0f, 4.0f };

    EXPECT_FLOAT_EQ (a[0], 1.0f);
    EXPECT_FLOAT_EQ (a[1], 2.0f);
    EXPECT_FLOAT_EQ (b[0], 3.0f);
    EXPECT_FLOAT_EQ (b[1], 4.0f);
}

TEST (Vector2f, ArithmeticOperators)
{
    Vector2f a (1.0f, 2.0f);
    Vector2f b (3.0f, -4.0f);

    EXPECT_VEC2_NEAR (a + b, Vector2f (4.0f, -2.0f));
    EXPECT_VEC2_NEAR (a - b, Vector2f (-2.0f, 6.0f));
    EXPECT_VEC2_NEAR (a * 2.0f, Vector2f (2.0f, 4.0f));
    EXPECT_VEC2_NEAR (2.0f * a, Vector2f (2.0f, 4.0f));
    EXPECT_VEC2_NEAR (b / 2.0f, Vector2f (1.5f, -2.0f));
    EXPECT_VEC2_NEAR (-a, Vector2f (-1.0f, -2.0f));
}

TEST (Vector2f, CopyAndMoveKeepValues)
{
    Vector2f a (5.0f, 6.0f);
    Vector2f copy (a);
    Vector2f moved (std::move (copy));
    Vector2f assigned;
    assigned = a;

    EXPECT_VEC2_NEAR (moved, a);
    EXPECT_VEC2_NEAR (assigned, a);
}

// ---------------------------------------------------------------------------
// Vector3f
// ---------------------------------------------------------------------------

TEST (Vector3f, DefaultConstructsToZero)
{
    Vector3f v;

    EXPECT_FLOAT_EQ (v[0], 0.0f);
    EXPECT_FLOAT_EQ (v[1], 0.0f);
    EXPECT_FLOAT_EQ (v[2], 0.0f);
    EXPECT_EQ (v.Dim (), 3u);
}

TEST (Vector3f, ConstructsFromComponentsInitializerListAndVector4f)
{
    Vector3f a (1.0f, 2.0f, 3.0f);
    Vector3f b { 4.0f, 5.0f, 6.0f };
    Vector3f c (Vector4f (7.0f, 8.0f, 9.0f, 10.0f));

    EXPECT_VEC3_NEAR (a, Vector3f (1.0f, 2.0f, 3.0f));
    EXPECT_VEC3_NEAR (b, Vector3f (4.0f, 5.0f, 6.0f));
    EXPECT_VEC3_NEAR (c, Vector3f (7.0f, 8.0f, 9.0f));
}

TEST (Vector3f, IndexingIsMutable)
{
    Vector3f v;
    v[0] = 1.0f;
    v[1] = 2.0f;
    v[2] = 3.0f;

    EXPECT_VEC3_NEAR (v, Vector3f (1.0f, 2.0f, 3.0f));
}

TEST (Vector3f, ArithmeticOperators)
{
    Vector3f a (1.0f, 2.0f, 3.0f);
    Vector3f b (-1.0f, 0.5f, 4.0f);

    EXPECT_VEC3_NEAR (a + b, Vector3f (0.0f, 2.5f, 7.0f));
    EXPECT_VEC3_NEAR (a - b, Vector3f (2.0f, 1.5f, -1.0f));
    EXPECT_VEC3_NEAR (a * 3.0f, Vector3f (3.0f, 6.0f, 9.0f));
    EXPECT_VEC3_NEAR (3.0f * a, Vector3f (3.0f, 6.0f, 9.0f));
    EXPECT_VEC3_NEAR (a / 2.0f, Vector3f (0.5f, 1.0f, 1.5f));
    EXPECT_VEC3_NEAR (-a, Vector3f (-1.0f, -2.0f, -3.0f));
}

TEST (Vector3f, CompoundAssignmentOperators)
{
    Vector3f v (1.0f, 2.0f, 3.0f);

    v += Vector3f (1.0f, 1.0f, 1.0f);
    EXPECT_VEC3_NEAR (v, Vector3f (2.0f, 3.0f, 4.0f));

    v -= Vector3f (2.0f, 2.0f, 2.0f);
    EXPECT_VEC3_NEAR (v, Vector3f (0.0f, 1.0f, 2.0f));

    v *= 4.0f;
    EXPECT_VEC3_NEAR (v, Vector3f (0.0f, 4.0f, 8.0f));

    v /= 2.0f;
    EXPECT_VEC3_NEAR (v, Vector3f (0.0f, 2.0f, 4.0f));
}

TEST (Vector3f, EqualityIsExact)
{
    EXPECT_TRUE (Vector3f (1.0f, 2.0f, 3.0f) == Vector3f (1.0f, 2.0f, 3.0f));
    EXPECT_FALSE (Vector3f (1.0f, 2.0f, 3.0f) == Vector3f (1.0f, 2.0f, 3.0001f));
}

TEST (Vector3f, CopyMoveAndAssignmentKeepValues)
{
    Vector3f a (1.0f, 2.0f, 3.0f);
    Vector3f copy (a);
    Vector3f moved (std::move (copy));
    Vector3f assigned;
    assigned = a;

    EXPECT_VEC3_NEAR (moved, a);
    EXPECT_VEC3_NEAR (assigned, a);
}

// ---------------------------------------------------------------------------
// Vector4f
// ---------------------------------------------------------------------------

TEST (Vector4f, DefaultConstructsToZero)
{
    Vector4f v;

    for (unsigned int i = 0; i < 4; i++)
    {
        EXPECT_FLOAT_EQ (v[i], 0.0f);
    }
    EXPECT_EQ (v.Dim (), 4u);
}

TEST (Vector4f, ConstructsFromVariousSources)
{
    Vector4f a (1.0f, 2.0f, 3.0f, 4.0f);
    Vector4f b (Vector3f (1.0f, 2.0f, 3.0f), 4.0f);
    Vector4f c { 1.0f, 2.0f, 3.0f, 4.0f };

    EXPECT_VEC4_NEAR (a, b);
    EXPECT_VEC4_NEAR (a, c);
}

TEST (Vector4f, ThreeComponentConstructorLeavesWDefined)
{
    // (x, y, z) constructor is used for directions/points; w must be a finite number
    Vector4f v (1.0f, 2.0f, 3.0f);

    EXPECT_FLOAT_EQ (v[0], 1.0f);
    EXPECT_FLOAT_EQ (v[1], 2.0f);
    EXPECT_FLOAT_EQ (v[2], 3.0f);
    EXPECT_TRUE (std::isfinite (v[3]));
}

TEST (Vector4f, ArithmeticOperators)
{
    Vector4f a (1.0f, 2.0f, 3.0f, 4.0f);
    Vector4f b (4.0f, 3.0f, 2.0f, 1.0f);

    EXPECT_VEC4_NEAR (a + b, Vector4f (5.0f, 5.0f, 5.0f, 5.0f));
    EXPECT_VEC4_NEAR (a - b, Vector4f (-3.0f, -1.0f, 1.0f, 3.0f));
    EXPECT_VEC4_NEAR (a * 2.0f, Vector4f (2.0f, 4.0f, 6.0f, 8.0f));
    EXPECT_VEC4_NEAR (0.5f * a, Vector4f (0.5f, 1.0f, 1.5f, 2.0f));
    EXPECT_VEC4_NEAR (a / 2.0f, Vector4f (0.5f, 1.0f, 1.5f, 2.0f));
    EXPECT_VEC4_NEAR (-a, Vector4f (-1.0f, -2.0f, -3.0f, -4.0f));
}

// ---------------------------------------------------------------------------
// Vector operations in Mathf
// ---------------------------------------------------------------------------

TEST (MathfVector, LengthOfKnownVectors)
{
    EXPECT_NEAR (Mathf::Length (Vector3f (3.0f, 4.0f, 0.0f)), 5.0f, 1e-5f);
    EXPECT_NEAR (Mathf::Length (Vector3f (1.0f, 2.0f, 2.0f)), 3.0f, 1e-5f);
    EXPECT_NEAR (Mathf::Length (Vector4f (1.0f, 2.0f, 2.0f, 4.0f)), 5.0f, 1e-5f);
    EXPECT_FLOAT_EQ (Mathf::Length (Vector3f ()), 0.0f);
}

TEST (MathfVector, NormalizeProducesUnitVectorInSameDirection)
{
    Vector3f v (3.0f, -4.0f, 12.0f);
    Vector3f n = Mathf::Normalize (v);

    EXPECT_NEAR (Mathf::Length (n), 1.0f, 1e-5f);
    EXPECT_VEC3_NEAR (n * Mathf::Length (v), v);

    Vector4f v4 (1.0f, 2.0f, 3.0f, 4.0f);
    EXPECT_NEAR (Mathf::Length (Mathf::Normalize (v4)), 1.0f, 1e-5f);
}

TEST (MathfVector, DotProduct)
{
    EXPECT_FLOAT_EQ (Mathf::Dot (Vector3f (1.0f, 2.0f, 3.0f), Vector3f (4.0f, -5.0f, 6.0f)), 12.0f);
    EXPECT_FLOAT_EQ (Mathf::Dot (Vector3f (1.0f, 0.0f, 0.0f), Vector3f (0.0f, 1.0f, 0.0f)), 0.0f);
}

TEST (MathfVector, CrossProductIsRightHanded)
{
    Vector3f x (1.0f, 0.0f, 0.0f);
    Vector3f y (0.0f, 1.0f, 0.0f);
    Vector3f z (0.0f, 0.0f, 1.0f);

    EXPECT_VEC3_NEAR (Mathf::Cross (x, y), z);
    EXPECT_VEC3_NEAR (Mathf::Cross (y, z), x);
    EXPECT_VEC3_NEAR (Mathf::Cross (z, x), y);
}

TEST (MathfVector, CrossProductIsAntiCommutativeAndOrthogonal)
{
    Vector3f a (1.0f, 2.0f, 3.0f);
    Vector3f b (-2.0f, 0.5f, 4.0f);
    Vector3f c = Mathf::Cross (a, b);

    EXPECT_VEC3_NEAR (c, -Mathf::Cross (b, a));
    EXPECT_NEAR (Mathf::Dot (c, a), 0.0f, 1e-4f);
    EXPECT_NEAR (Mathf::Dot (c, b), 0.0f, 1e-4f);
    EXPECT_VEC3_NEAR (Mathf::Cross (a, a), Vector3f (0.0f, 0.0f, 0.0f));
}

TEST (MathfVector, HomogenousCoordinatesRoundTrip)
{
    Vector3f p (2.0f, 4.0f, 6.0f);
    Vector4f h = Mathf::CartesianToHomogenous (p, 2.0f);

    // homogenous point is (w * p, w)
    EXPECT_VEC4_NEAR (h, Vector4f (4.0f, 8.0f, 12.0f, 2.0f));
    EXPECT_FLOAT_EQ (Mathf::GetHomogenousWeight (h), 2.0f);
    EXPECT_VEC3_NEAR (Mathf::HomogenousToCartesianPerspective (h), p);
    EXPECT_VEC3_NEAR (Mathf::HomogenousToCartesianTruncate (h), Vector3f (4.0f, 8.0f, 12.0f));
}

TEST (MathfVector, HomogenousVectorConversion)
{
    std::vector<Vector3f> points = { Vector3f (1.0f, 2.0f, 3.0f), Vector3f (4.0f, 5.0f, 6.0f) };
    std::vector<float> weights = { 1.0f, 0.5f };

    auto h = Mathf::CartesianToHomogenous (points, weights);

    ASSERT_EQ (h.size (), 2u);
    EXPECT_VEC4_NEAR (h[0], Vector4f (1.0f, 2.0f, 3.0f, 1.0f));
    EXPECT_VEC4_NEAR (h[1], Vector4f (2.0f, 2.5f, 3.0f, 0.5f));
}
