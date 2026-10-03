#include "TestHelpers.h"
#include "math/Triangle.h"
#include <cmath>
#include <limits>
#include <vector>

using namespace cilantro;
using namespace cilantro::testing;

namespace {

const float kPi = Mathf::Pi ();

float TriangleArea (const Triangle<Vector3f>& t)
{
    return 0.5f * Mathf::Length (Mathf::Cross (t[1] - t[0], t[2] - t[0]));
}

float TotalArea (const std::vector<Triangle<Vector3f>>& triangles)
{
    float area = 0.0f;
    for (const auto& t : triangles)
    {
        area += TriangleArea (t);
    }
    return area;
}

} // namespace

// ---------------------------------------------------------------------------
// Scalars and comparison
// ---------------------------------------------------------------------------

TEST (Mathf, PiConstant)
{
    EXPECT_NEAR (Mathf::Pi (), 3.14159265f, 1e-7f);
}

TEST (Mathf, VeryCloseAcceptsRoundingErrorsOnly)
{
    EXPECT_TRUE (Mathf::VeryClose (1.0f, 1.0f, 2));
    EXPECT_TRUE (Mathf::VeryClose (0.1f + 0.2f, 0.3f, 4));
    EXPECT_FALSE (Mathf::VeryClose (1.0f, 1.001f, 4));
    EXPECT_FALSE (Mathf::VeryClose (1.0f, 2.0f, 4));
}

TEST (Mathf, DegreesAndRadiansConversion)
{
    EXPECT_NEAR (Mathf::Deg2Rad (180.0f), kPi, 1e-6f);
    EXPECT_NEAR (Mathf::Deg2Rad (90.0f), kPi * 0.5f, 1e-6f);
    EXPECT_NEAR (Mathf::Rad2Deg (kPi), 180.0f, 1e-4f);
    EXPECT_NEAR (Mathf::Rad2Deg (Mathf::Deg2Rad (37.5f)), 37.5f, 1e-4f);

    EXPECT_VEC3_NEAR (Mathf::Deg2Rad (Vector3f (90.0f, 180.0f, 360.0f)), Vector3f (kPi * 0.5f, kPi, kPi * 2.0f));
    EXPECT_VEC3_NEAR (Mathf::Rad2Deg (Vector3f (kPi, kPi * 0.5f, 0.0f)), Vector3f (180.0f, 90.0f, 0.0f));
}

TEST (Mathf, BinomialCoefficients)
{
    EXPECT_EQ (Mathf::Binomial (5, 0), 1u);
    EXPECT_EQ (Mathf::Binomial (5, 1), 5u);
    EXPECT_EQ (Mathf::Binomial (5, 2), 10u);
    EXPECT_EQ (Mathf::Binomial (5, 3), 10u);
    EXPECT_EQ (Mathf::Binomial (5, 5), 1u);
    EXPECT_EQ (Mathf::Binomial (10, 4), 210u);
    EXPECT_EQ (Mathf::Binomial (30, 15), 155117520u);
}

TEST (Mathf, BinomialIsSymmetricAndFollowsPascalsRule)
{
    for (unsigned int n = 2; n <= 20; n++)
    {
        for (unsigned int k = 1; k < n; k++)
        {
            EXPECT_EQ (Mathf::Binomial (n, k), Mathf::Binomial (n, n - k));
            EXPECT_EQ (Mathf::Binomial (n, k), Mathf::Binomial (n - 1, k - 1) + Mathf::Binomial (n - 1, k));
        }
    }
}

TEST (Mathf, BinomialReturnsZeroOnOverflow)
{
    // C(100, 50) does not fit in 32 bits
    EXPECT_EQ (Mathf::Binomial (100, 50), 0u);
}

// ---------------------------------------------------------------------------
// Interpolation
// ---------------------------------------------------------------------------

TEST (MathfInterpolation, ClampTakesValueFirst)
{
    // signature is Clamp (value, min, max)
    EXPECT_FLOAT_EQ (Mathf::Clamp (5.0f, 0.0f, 1.0f), 1.0f);
    EXPECT_FLOAT_EQ (Mathf::Clamp (-5.0f, 0.0f, 1.0f), 0.0f);
    EXPECT_FLOAT_EQ (Mathf::Clamp (0.25f, 0.0f, 1.0f), 0.25f);
}

TEST (MathfInterpolation, StepIsLinearRampBetweenEdges)
{
    EXPECT_FLOAT_EQ (Mathf::Step (2.0f, 4.0f, 1.0f), 0.0f);
    EXPECT_FLOAT_EQ (Mathf::Step (2.0f, 4.0f, 2.0f), 0.0f);
    EXPECT_FLOAT_EQ (Mathf::Step (2.0f, 4.0f, 3.0f), 0.5f);
    EXPECT_FLOAT_EQ (Mathf::Step (2.0f, 4.0f, 4.0f), 1.0f);
    EXPECT_FLOAT_EQ (Mathf::Step (2.0f, 4.0f, 9.0f), 1.0f);
}

TEST (MathfInterpolation, SmoothstepHasZeroSlopeAtEdges)
{
    EXPECT_FLOAT_EQ (Mathf::Smoothstep (0.0f, 1.0f, 0.0f), 0.0f);
    EXPECT_FLOAT_EQ (Mathf::Smoothstep (0.0f, 1.0f, 1.0f), 1.0f);
    EXPECT_NEAR (Mathf::Smoothstep (0.0f, 1.0f, 0.5f), 0.5f, 1e-6f);
    EXPECT_NEAR (Mathf::Smoothstep (0.0f, 1.0f, 0.25f), 0.15625f, 1e-6f);

    // flatter than a linear ramp near the edges
    EXPECT_LT (Mathf::Smoothstep (0.0f, 1.0f, 0.1f), 0.1f);
    EXPECT_GT (Mathf::Smoothstep (0.0f, 1.0f, 0.9f), 0.9f);
}

TEST (MathfInterpolation, SmootherstepEdgesAndMidpoint)
{
    EXPECT_FLOAT_EQ (Mathf::Smootherstep (0.0f, 1.0f, 0.0f), 0.0f);
    EXPECT_FLOAT_EQ (Mathf::Smootherstep (0.0f, 1.0f, 1.0f), 1.0f);
    EXPECT_NEAR (Mathf::Smootherstep (0.0f, 1.0f, 0.5f), 0.5f, 1e-6f);
    EXPECT_FLOAT_EQ (Mathf::Smootherstep (0.0f, 1.0f, 5.0f), 1.0f);
}

TEST (MathfInterpolation, ScalarLerpClampsParameter)
{
    EXPECT_FLOAT_EQ (Mathf::Lerp (10.0f, 20.0f, 0.0f), 10.0f);
    EXPECT_FLOAT_EQ (Mathf::Lerp (10.0f, 20.0f, 0.5f), 15.0f);
    EXPECT_FLOAT_EQ (Mathf::Lerp (10.0f, 20.0f, 1.0f), 20.0f);
    EXPECT_FLOAT_EQ (Mathf::Lerp (10.0f, 20.0f, -1.0f), 10.0f);
    EXPECT_FLOAT_EQ (Mathf::Lerp (10.0f, 20.0f, 2.0f), 20.0f);
}

TEST (MathfInterpolation, VectorLerp)
{
    Vector3f a (0.0f, 10.0f, -4.0f);
    Vector3f b (4.0f, 20.0f, 4.0f);

    EXPECT_VEC3_NEAR (Mathf::Lerp (a, b, 0.25f), Vector3f (1.0f, 12.5f, -2.0f));
    EXPECT_VEC3_NEAR (Mathf::Lerp (a, b, 0.0f), a);
    EXPECT_VEC3_NEAR (Mathf::Lerp (a, b, 1.0f), b);
    EXPECT_VEC3_NEAR (Mathf::Lerp (a, b, 3.0f), b);
}

// ---------------------------------------------------------------------------
// Coordinates
// ---------------------------------------------------------------------------

TEST (MathfCoordinates, SphericalToCartesian3D)
{
    // theta is measured from +Y axis, phi around it from +X
    EXPECT_VEC3_NEAR (Mathf::Spherical2Cartesian (0.0f, 0.0f, 2.0f), Vector3f (0.0f, 2.0f, 0.0f));
    EXPECT_VEC3_NEAR (Mathf::Spherical2Cartesian (kPi * 0.5f, 0.0f, 2.0f), Vector3f (2.0f, 0.0f, 0.0f));
    EXPECT_VEC3_NEAR (Mathf::Spherical2Cartesian (kPi * 0.5f, kPi * 0.5f, 2.0f), Vector3f (0.0f, 0.0f, 2.0f));
    EXPECT_NEAR (Mathf::Length (Mathf::Spherical2Cartesian (0.7f, 1.9f, 3.0f)), 3.0f, 1e-4f);
}

TEST (MathfCoordinates, PolarToCartesian2D)
{
    EXPECT_VEC2_NEAR (Mathf::Spherical2Cartesian (0.0f, 3.0f), Vector2f (3.0f, 0.0f));
    EXPECT_VEC2_NEAR (Mathf::Spherical2Cartesian (kPi * 0.5f, 3.0f), Vector2f (0.0f, 3.0f));
    EXPECT_VEC2_NEAR (Mathf::Spherical2Cartesian (kPi, 1.0f), Vector2f (-1.0f, 0.0f));
}

// ---------------------------------------------------------------------------
// Numerical routines
// ---------------------------------------------------------------------------

TEST (MathfNumerical, IntegralOfPolynomial)
{
    EXPECT_NEAR (Mathf::Integral (0.0f, 1.0f, [](float x) { return x * x; }), 1.0f / 3.0f, 1e-4f);
    EXPECT_NEAR (Mathf::Integral (-1.0f, 2.0f, [](float x) { return 3.0f * x * x; }), 9.0f, 1e-3f);
}

TEST (MathfNumerical, IntegralOfTrigonometricFunctions)
{
    EXPECT_NEAR (Mathf::Integral (0.0f, kPi, [](float x) { return std::sin (x); }), 2.0f, 1e-3f);
    EXPECT_NEAR (Mathf::Integral (0.0f, kPi * 0.5f, [](float x) { return std::cos (x); }), 1.0f, 1e-3f);
}

TEST (MathfNumerical, IntegralOfConstantIsAreaAndZeroWidthIsZero)
{
    EXPECT_NEAR (Mathf::Integral (1.0f, 4.0f, [](float) { return 2.0f; }), 6.0f, 1e-4f);
    EXPECT_NEAR (Mathf::Integral (2.0f, 2.0f, [](float x) { return x; }), 0.0f, 1e-5f);
}

TEST (MathfNumerical, SolvesLinearSystemWithScalarRightHandSide)
{
    // x + y + z = 6, 2y + 5z = -4, 2x + 5y - z = 27  ->  x = 5, y = 3, z = -2
    std::vector<std::vector<float>> a = {
        { 1.0f, 1.0f, 1.0f },
        { 0.0f, 2.0f, 5.0f },
        { 2.0f, 5.0f, -1.0f }
    };
    std::vector<float> b = { 6.0f, -4.0f, 27.0f };

    Mathf::SolveSystemOfLinearEquations (a, b);

    EXPECT_NEAR (b[0], 5.0f, 1e-4f);
    EXPECT_NEAR (b[1], 3.0f, 1e-4f);
    EXPECT_NEAR (b[2], -2.0f, 1e-4f);
}

TEST (MathfNumerical, SolvesLinearSystemWithVectorRightHandSide)
{
    // each component of the Vector3f right-hand side is solved independently
    std::vector<std::vector<float>> a = {
        { 2.0f, 0.0f },
        { 0.0f, 4.0f }
    };
    std::vector<Vector3f> b = { Vector3f (2.0f, 4.0f, 6.0f), Vector3f (4.0f, 8.0f, 12.0f) };

    Mathf::SolveSystemOfLinearEquations (a, b);

    EXPECT_VEC3_NEAR (b[0], Vector3f (1.0f, 2.0f, 3.0f));
    EXPECT_VEC3_NEAR (b[1], Vector3f (1.0f, 2.0f, 3.0f));
}

TEST (MathfNumerical, LinearSystemNeedsPivoting)
{
    // zero on the diagonal requires a row swap
    std::vector<std::vector<float>> a = {
        { 0.0f, 1.0f },
        { 1.0f, 0.0f }
    };
    std::vector<float> b = { 7.0f, 3.0f };

    Mathf::SolveSystemOfLinearEquations (a, b);

    EXPECT_NEAR (b[0], 3.0f, 1e-5f);
    EXPECT_NEAR (b[1], 7.0f, 1e-5f);
}

TEST (MathfNumerical, LinearSystemWithMismatchedSizesIsLeftUntouched)
{
    std::vector<std::vector<float>> a = {
        { 1.0f, 0.0f },
        { 0.0f, 1.0f }
    };
    std::vector<float> b = { 1.0f, 2.0f, 3.0f };

    Mathf::SolveSystemOfLinearEquations (a, b);

    EXPECT_FLOAT_EQ (b[0], 1.0f);
    EXPECT_FLOAT_EQ (b[1], 2.0f);
    EXPECT_FLOAT_EQ (b[2], 3.0f);
}

// ---------------------------------------------------------------------------
// Intersections
// ---------------------------------------------------------------------------

TEST (MathfIntersection, LineWithAxisPlanes)
{
    Vector3f p0 (0.0f, 0.0f, 0.0f);
    Vector3f p1 (2.0f, 4.0f, 6.0f);

    EXPECT_VEC3_NEAR (Mathf::IntersectWithXPlane (p0, p1, 1.0f), Vector3f (1.0f, 2.0f, 3.0f));
    EXPECT_VEC3_NEAR (Mathf::IntersectWithYPlane (p0, p1, 2.0f), Vector3f (1.0f, 2.0f, 3.0f));
    EXPECT_VEC3_NEAR (Mathf::IntersectWithZPlane (p0, p1, 3.0f), Vector3f (1.0f, 2.0f, 3.0f));
}

TEST (MathfIntersection, LineWithAxisPlaneOutsideSegmentExtrapolates)
{
    EXPECT_VEC3_NEAR (Mathf::IntersectWithXPlane (Vector3f (0, 0, 0), Vector3f (1, 1, 1), 3.0f), Vector3f (3, 3, 3));
}

TEST (MathfIntersection, LineWithArbitraryPlane)
{
    Vector3f normal = Mathf::Normalize (Vector3f (1.0f, 1.0f, 0.0f));
    Vector3f pointOnPlane (1.0f, 1.0f, 0.0f);

    Vector3f hit = Mathf::IntersectWithPlane (Vector3f (0, 0, 0), Vector3f (4, 4, 4), normal, pointOnPlane);

    // plane x + y = 2 intersected with the line (t, t, t)
    EXPECT_VEC3_NEAR (hit, Vector3f (1.0f, 1.0f, 1.0f));
    EXPECT_NEAR (Mathf::Dot (hit - pointOnPlane, normal), 0.0f, 1e-5f);
}

// ---------------------------------------------------------------------------
// Triangle clipping
// ---------------------------------------------------------------------------

TEST (MathfClipping, TriangleInsideIsUntouched)
{
    std::vector<Triangle<Vector3f>> triangles = { Triangle<Vector3f> (Vector3f (0, 0, 0), Vector3f (1, 0, 0), Vector3f (0, 1, 0)) };

    Mathf::ClipTrianglesToPlanes (triangles, Vector3f (-5, -5, -5), Vector3f (5, 5, 5));

    ASSERT_EQ (triangles.size (), 1u);
    EXPECT_VEC3_NEAR (triangles[0][0], Vector3f (0, 0, 0));
    EXPECT_VEC3_NEAR (triangles[0][1], Vector3f (1, 0, 0));
    EXPECT_VEC3_NEAR (triangles[0][2], Vector3f (0, 1, 0));
}

TEST (MathfClipping, TriangleCompletelyOutsideIsCulled)
{
    std::vector<Triangle<Vector3f>> triangles = { Triangle<Vector3f> (Vector3f (10, 0, 0), Vector3f (11, 0, 0), Vector3f (10, 1, 0)) };

    Mathf::ClipTrianglesToPlanes (triangles, Vector3f (-5, -5, -5), Vector3f (5, 5, 5));

    EXPECT_TRUE (triangles.empty ());
}

TEST (MathfClipping, EmptyInputStaysEmpty)
{
    std::vector<Triangle<Vector3f>> triangles;

    Mathf::ClipTrianglesToPlanes (triangles, Vector3f (-1, -1, -1), Vector3f (1, 1, 1));

    EXPECT_TRUE (triangles.empty ());
}

TEST (MathfClipping, TriangleWithOneVertexOutsideIsClipped)
{
    // right triangle with area 2, the part with x > 1 (area 0.5) must be removed
    std::vector<Triangle<Vector3f>> triangles = { Triangle<Vector3f> (Vector3f (0, 0, 0), Vector3f (2, 0, 0), Vector3f (0, 2, 0)) };

    Mathf::ClipTrianglesToPlanes (triangles, Vector3f (-5, -5, -5), Vector3f (1, 5, 5));

    EXPECT_NEAR (TotalArea (triangles), 1.5f, 1e-4f);
    for (const auto& t : triangles)
    {
        for (unsigned int v = 0; v < 3; v++)
        {
            EXPECT_LE (t[v][0], 1.0f + 1e-4f);
        }
    }
}

TEST (MathfClipping, TriangleWithTwoVerticesOutsideIsClipped)
{
    // vertices (0, 0) and (0, 2) are below the minimum plane x = 1, only (2, 0) is inside;
    // what remains is the triangle (1, 0), (2, 0), (1, 1) with area 0.5
    std::vector<Triangle<Vector3f>> triangles = { Triangle<Vector3f> (Vector3f (0, 0, 0), Vector3f (2, 0, 0), Vector3f (0, 2, 0)) };

    Mathf::ClipTrianglesToPlanes (triangles, Vector3f (1, -5, -5), Vector3f (5, 5, 5));

    EXPECT_NEAR (TotalArea (triangles), 0.5f, 1e-4f);
    for (const auto& t : triangles)
    {
        for (unsigned int v = 0; v < 3; v++)
        {
            EXPECT_GE (t[v][0], 1.0f - 1e-4f);
        }
    }
}

TEST (MathfClipping, ClippingAgainstAllSixPlanesKeepsGeometryInsideBox)
{
    // large triangle spanning past the box on several sides
    std::vector<Triangle<Vector3f>> triangles = { Triangle<Vector3f> (Vector3f (-10, -10, 0), Vector3f (10, -10, 0), Vector3f (0, 10, 0)) };

    Mathf::ClipTrianglesToPlanes (triangles, Vector3f (-1, -1, -1), Vector3f (1, 1, 1));

    ASSERT_FALSE (triangles.empty ());
    for (const auto& t : triangles)
    {
        for (unsigned int v = 0; v < 3; v++)
        {
            EXPECT_GE (t[v][0], -1.0f - 1e-4f);
            EXPECT_LE (t[v][0], 1.0f + 1e-4f);
            EXPECT_GE (t[v][1], -1.0f - 1e-4f);
            EXPECT_LE (t[v][1], 1.0f + 1e-4f);
        }
    }
    // the whole 2x2 box cross-section lies inside the big triangle
    EXPECT_NEAR (TotalArea (triangles), 4.0f, 1e-3f);
}

// ---------------------------------------------------------------------------
// Triangle container
// ---------------------------------------------------------------------------

TEST (Triangle, StoresThreeVerticesAndSupportsIndexing)
{
    Triangle<Vector3f> t (Vector3f (1, 0, 0), Vector3f (0, 1, 0), Vector3f (0, 0, 1));

    EXPECT_VEC3_NEAR (t[0], Vector3f (1, 0, 0));
    EXPECT_VEC3_NEAR (t[1], Vector3f (0, 1, 0));
    EXPECT_VEC3_NEAR (t[2], Vector3f (0, 0, 1));

    t[1] = Vector3f (5, 5, 5);
    EXPECT_VEC3_NEAR (t[1], Vector3f (5, 5, 5));
}

TEST (Triangle, CopyKeepsVertices)
{
    Triangle<Vector3f> a (Vector3f (1, 2, 3), Vector3f (4, 5, 6), Vector3f (7, 8, 9));
    Triangle<Vector3f> b (a);

    EXPECT_VEC3_NEAR (b[2], Vector3f (7, 8, 9));
}
