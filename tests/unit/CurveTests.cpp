#include "TestHelpers.h"
#include "math/Bezier.h"
#include "math/BSpline.h"
#include "math/NURBS.h"
#include "math/CubicHermite.h"
#include <vector>

using namespace cilantro;
using namespace cilantro::testing;

namespace {

// central difference approximation of the curve derivative
template <typename Curve>
Vector3f NumericalTangent (const Curve& curve, float t, float h = 1e-3f)
{
    return (curve.GetCurvePoint (t + h) - curve.GetCurvePoint (t - h)) / (2.0f * h);
}

const std::vector<Vector3f> kQuadraticPoints = { Vector3f (0, 0, 0), Vector3f (1, 2, 0), Vector3f (3, 0, 1) };
const std::vector<Vector3f> kCubicPoints = { Vector3f (0, 0, 0), Vector3f (1, 3, 0), Vector3f (3, 3, 2), Vector3f (4, 0, 1) };

} // namespace

// ---------------------------------------------------------------------------
// Bezier
// ---------------------------------------------------------------------------

TEST (Bezier, ValidatesNumberOfPointsAgainstDegree)
{
    Bezier<Vector3f, 2> quadratic (kQuadraticPoints);
    EXPECT_TRUE (quadratic.Validate ());

    Bezier<Vector3f, 2> wrong (kCubicPoints);
    EXPECT_FALSE (wrong.Validate ());

    Bezier<Vector3f, 3> cubic (kCubicPoints);
    EXPECT_TRUE (cubic.Validate ());
}

TEST (Bezier, CurveInterpolatesFirstAndLastPoint)
{
    Bezier<Vector3f, 2> quadratic (kQuadraticPoints);
    Bezier<Vector3f, 3> cubic (kCubicPoints);

    EXPECT_VEC3_NEAR (quadratic.GetCurvePoint (0.0f), kQuadraticPoints.front ());
    EXPECT_VEC3_NEAR (quadratic.GetCurvePoint (1.0f), kQuadraticPoints.back ());
    EXPECT_VEC3_NEAR (cubic.GetCurvePoint (0.0f), kCubicPoints.front ());
    EXPECT_VEC3_NEAR (cubic.GetCurvePoint (1.0f), kCubicPoints.back ());
}

TEST (Bezier, QuadraticMidpointFollowsBernsteinWeights)
{
    Bezier<Vector3f, 2> curve (kQuadraticPoints);

    Vector3f expected = 0.25f * kQuadraticPoints[0] + 0.5f * kQuadraticPoints[1] + 0.25f * kQuadraticPoints[2];

    EXPECT_VEC3_NEAR (curve.GetCurvePoint (0.5f), expected);
}

TEST (Bezier, CubicMidpointFollowsBernsteinWeights)
{
    Bezier<Vector3f, 3> curve (kCubicPoints);

    Vector3f expected = (kCubicPoints[0] + 3.0f * kCubicPoints[1] + 3.0f * kCubicPoints[2] + kCubicPoints[3]) / 8.0f;

    EXPECT_VEC3_NEAR (curve.GetCurvePoint (0.5f), expected);
}

TEST (Bezier, SetPointsReplacesControlPoints)
{
    Bezier<Vector3f, 2> curve;
    curve.SetPoints (kQuadraticPoints);

    EXPECT_TRUE (curve.Validate ());
    EXPECT_VEC3_NEAR (curve.GetCurvePoint (1.0f), kQuadraticPoints.back ());
}

TEST (Bezier, QuadraticTangentIsLinearBlendOfControlPolygonEdges)
{
    Bezier<Vector3f, 2> curve (kQuadraticPoints);

    for (float t : { 0.0f, 0.25f, 0.5f, 1.0f })
    {
        Vector3f expected = 2.0f * ((1.0f - t) * (kQuadraticPoints[1] - kQuadraticPoints[0]) + t * (kQuadraticPoints[2] - kQuadraticPoints[1]));

        EXPECT_VEC3_NEAR (curve.GetCurveTangent (t), expected);
    }
}

TEST (Bezier, QuadraticTangentMatchesNumericalDerivative)
{
    Bezier<Vector3f, 2> curve (kQuadraticPoints);

    EXPECT_VEC3_NEAR (curve.GetCurveTangent (0.4f), NumericalTangent (curve, 0.4f), 1e-2f);
}

TEST (Bezier, CubicTangentAtEndsIsThreeTimesFirstAndLastEdge)
{
    Bezier<Vector3f, 3> curve (kCubicPoints);

    EXPECT_VEC3_NEAR (curve.GetCurveTangent (0.0f), 3.0f * (kCubicPoints[1] - kCubicPoints[0]));
    EXPECT_VEC3_NEAR (curve.GetCurveTangent (1.0f), 3.0f * (kCubicPoints[3] - kCubicPoints[2]));
}

TEST (Bezier, CubicTangentMatchesNumericalDerivative)
{
    Bezier<Vector3f, 3> curve (kCubicPoints);

    for (float t : { 0.2f, 0.5f, 0.8f })
    {
        EXPECT_VEC3_NEAR (curve.GetCurveTangent (t), NumericalTangent (curve, t), 1e-2f);
    }
}

TEST (Bezier, LengthOfStraightCurveIsDistanceBetweenEndPoints)
{
    Bezier<Vector3f, 2> line (std::vector<Vector3f> { Vector3f (0, 0, 0), Vector3f (1, 0, 0), Vector3f (2, 0, 0) });

    EXPECT_NEAR (line.GetCurveLength (), 2.0f, 1e-3f);
}

// ---------------------------------------------------------------------------
// BSpline
// ---------------------------------------------------------------------------

TEST (BSpline, ControlPointsAreStoredInOrder)
{
    BSpline<Vector3f, 2> curve;
    curve.AddControlPoint (Vector3f (1, 0, 0));
    curve.AddControlPoint (Vector3f (2, 0, 0));

    EXPECT_EQ (curve.GetControlPointsCount (), 2u);
    EXPECT_VEC3_NEAR (curve.GetControlPoint (0), Vector3f (1, 0, 0));
    EXPECT_VEC3_NEAR (curve.GetControlPoint (1), Vector3f (2, 0, 0));

    curve.AddControlPoints (kCubicPoints);
    EXPECT_EQ (curve.GetControlPointsCount (), kCubicPoints.size ());
}

TEST (BSpline, ClampedKnotVectorValidatesAndHasExpectedShape)
{
    BSpline<Vector3f, 2> curve;
    curve.AddControlPoints (kCubicPoints);
    curve.CalculateKnotVector (KnotVectorType::Clamped);

    // n control points and degree p need n + p + 1 knots
    EXPECT_TRUE (curve.Validate ());
}

TEST (BSpline, ValidateFailsForMismatchedKnotVector)
{
    BSpline<Vector3f, 2> curve;
    curve.AddControlPoints (kCubicPoints);
    curve.SetKnotVector ({ 0.0f, 0.0f, 0.0f, 1.0f, 1.0f });

    EXPECT_FALSE (curve.Validate ());
}

TEST (BSpline, UniformKnotVectorValidates)
{
    BSpline<Vector3f, 3> curve;
    curve.AddControlPoints (std::vector<Vector3f> { Vector3f (0, 0, 0), Vector3f (1, 1, 0), Vector3f (2, 0, 0), Vector3f (3, 1, 0), Vector3f (4, 0, 0) });
    curve.CalculateKnotVector (KnotVectorType::Uniform);

    EXPECT_TRUE (curve.Validate ());
}

TEST (BSpline, ClampedCurveInterpolatesEndPoints)
{
    BSpline<Vector3f, 2> curve;
    curve.AddControlPoints (kCubicPoints);
    curve.CalculateKnotVector ();

    EXPECT_VEC3_NEAR (curve.GetCurvePoint (0.0f), kCubicPoints.front ());
    EXPECT_VEC3_NEAR (curve.GetCurvePoint (1.0f), kCubicPoints.back ());
}

TEST (BSpline, CurveWithCollinearControlPointsStaysOnTheLine)
{
    BSpline<Vector3f, 3> curve;
    curve.AddControlPoints (std::vector<Vector3f> { Vector3f (0, 0, 0), Vector3f (1, 1, 1), Vector3f (2, 2, 2), Vector3f (3, 3, 3), Vector3f (4, 4, 4) });
    curve.CalculateKnotVector ();

    for (float t = 0.05f; t < 1.0f; t += 0.1f)
    {
        Vector3f p = curve.GetCurvePoint (t);

        EXPECT_NEAR (p[0], p[1], 1e-4f);
        EXPECT_NEAR (p[1], p[2], 1e-4f);
    }
}

TEST (BSpline, CurveStaysInsideConvexHullOfControlPoints)
{
    BSpline<Vector3f, 3> curve;
    curve.AddControlPoints (std::vector<Vector3f> { Vector3f (0, 0, 0), Vector3f (1, 3, 0), Vector3f (2, -1, 0), Vector3f (3, 2, 0), Vector3f (4, 0, 0) });
    curve.CalculateKnotVector ();

    for (float t = 0.0f; t <= 1.0f; t += 0.05f)
    {
        Vector3f p = curve.GetCurvePoint (t);

        EXPECT_GE (p[0], -1e-4f);
        EXPECT_LE (p[0], 4.0f + 1e-4f);
        EXPECT_GE (p[1], -1.0f - 1e-4f);
        EXPECT_LE (p[1], 3.0f + 1e-4f);
    }
}

TEST (BSpline, BasisFunctionsFormPartitionOfUnity)
{
    // if all control points are equal, the curve is that point everywhere - this holds only if
    // the basis functions sum to one over the whole parameter range
    const Vector3f point (2.0f, 3.0f, 4.0f);

    BSpline<Vector3f, 2> curve;
    curve.AddControlPoints (std::vector<Vector3f> (6, point));
    curve.CalculateKnotVector ();

    for (float t : { 0.0f, 0.1f, 0.3f, 0.5f, 0.77f, 0.95f, 1.0f })
    {
        EXPECT_VEC3_NEAR (curve.GetCurvePoint (t), point);
    }
}

TEST (BSpline, TangentMatchesNumericalDerivative)
{
    BSpline<Vector3f, 3> curve;
    curve.AddControlPoints (std::vector<Vector3f> { Vector3f (0, 0, 0), Vector3f (1, 3, 0), Vector3f (2, -1, 1), Vector3f (3, 2, 0), Vector3f (4, 0, 2) });
    curve.CalculateKnotVector ();

    for (float t : { 0.2f, 0.5f, 0.8f })
    {
        EXPECT_VEC3_NEAR (curve.GetCurveTangent (t), NumericalTangent (curve, t), 2e-2f);
    }
}

TEST (BSpline, LengthOfStraightCurveIsDistanceBetweenEndPoints)
{
    BSpline<Vector3f, 2> curve;
    curve.AddControlPoints (std::vector<Vector3f> { Vector3f (0, 0, 0), Vector3f (1, 0, 0), Vector3f (2, 0, 0), Vector3f (3, 0, 0) });
    curve.CalculateKnotVector ();

    EXPECT_NEAR (curve.GetCurveLength (), 3.0f, 1e-2f);
}

// ---------------------------------------------------------------------------
// NURBS
// ---------------------------------------------------------------------------

TEST (NURBS, UnitWeightsReproduceBSpline)
{
    BSpline<Vector3f, 2> bspline;
    bspline.AddControlPoints (kCubicPoints);
    bspline.CalculateKnotVector ();

    NURBS<Vector3f, 2> nurbs;
    nurbs.AddControlPoints (kCubicPoints);
    nurbs.CalculateKnotVector ();
    nurbs.SetWeights ({ 1.0f, 1.0f, 1.0f, 1.0f });

    for (float t : { 0.0f, 0.2f, 0.5f, 0.9f, 1.0f })
    {
        EXPECT_VEC3_NEAR (nurbs.GetCurvePoint (t), bspline.GetCurvePoint (t));
    }
}

TEST (NURBS, InterpolatesEndPointsRegardlessOfWeights)
{
    NURBS<Vector3f, 2> nurbs;
    nurbs.AddControlPoints (kQuadraticPoints);
    nurbs.CalculateKnotVector ();
    nurbs.SetWeights ({ 1.0f, 5.0f, 1.0f });

    EXPECT_VEC3_NEAR (nurbs.GetCurvePoint (0.0f), kQuadraticPoints.front ());
    EXPECT_VEC3_NEAR (nurbs.GetCurvePoint (1.0f), kQuadraticPoints.back ());
}

TEST (NURBS, WeightPullsCurveTowardsControlPoint)
{
    // quadratic Bezier-like NURBS: C(0.5) = (P0 + 2 w P1 + P2) / (2 + 2 w)
    const std::vector<Vector3f> points = { Vector3f (0, 0, 0), Vector3f (1, 1, 0), Vector3f (2, 0, 0) };

    NURBS<Vector3f, 2> light;
    light.AddControlPoints (points);
    light.CalculateKnotVector ();
    light.SetWeights ({ 1.0f, 1.0f, 1.0f });

    NURBS<Vector3f, 2> heavy;
    heavy.AddControlPoints (points);
    heavy.CalculateKnotVector ();
    heavy.SetWeights ({ 1.0f, 4.0f, 1.0f });

    // at the midpoint the curve is at height 0.5 for unit weights and (2 * 4) / (2 + 2 * 4) = 0.8 for weight 4
    EXPECT_NEAR (light.GetCurvePoint (0.5f)[1], 0.5f, 1e-4f);
    EXPECT_NEAR (heavy.GetCurvePoint (0.5f)[1], 0.8f, 1e-4f);
}

TEST (NURBS, ValidatesWeightsCount)
{
    NURBS<Vector3f, 2> nurbs;
    nurbs.AddControlPoints (kCubicPoints);
    nurbs.CalculateKnotVector ();

    nurbs.SetWeights ({ 1.0f, 1.0f, 1.0f, 1.0f });
    EXPECT_TRUE (nurbs.Validate ());

    nurbs.SetWeights ({ 1.0f, 1.0f });
    EXPECT_FALSE (nurbs.Validate ());
}

// ---------------------------------------------------------------------------
// CubicHermite
// ---------------------------------------------------------------------------

TEST (CubicHermite, InterpolatesEndPoints)
{
    CubicHermite<Vector3f> curve (Vector3f (0, 0, 0), Vector3f (4, 2, 0), Vector3f (1, 0, 0), Vector3f (0, 1, 0));

    EXPECT_VEC3_NEAR (curve.GetCurvePoint (0.0f), Vector3f (0, 0, 0));
    EXPECT_VEC3_NEAR (curve.GetCurvePoint (1.0f), Vector3f (4, 2, 0));
}

TEST (CubicHermite, TangentsAtEndsMatchGivenTangents)
{
    CubicHermite<Vector3f> curve (Vector3f (0, 0, 0), Vector3f (4, 2, 0), Vector3f (1, 0, 0), Vector3f (0, 1, 0));

    EXPECT_VEC3_NEAR (curve.GetCurveTangent (0.0f), Vector3f (1, 0, 0));
    EXPECT_VEC3_NEAR (curve.GetCurveTangent (1.0f), Vector3f (0, 1, 0));
}

TEST (CubicHermite, MidpointFollowsHermiteBasis)
{
    Vector3f p0 (0, 0, 0);
    Vector3f p1 (4, 2, 0);
    Vector3f m0 (1, 0, 0);
    Vector3f m1 (0, 1, 0);
    CubicHermite<Vector3f> curve (p0, p1, m0, m1);

    // h00 = h10... at t = 0.5: 0.5 p0 + 0.125 m0 + 0.5 p1 - 0.125 m1
    Vector3f expected = 0.5f * p0 + 0.125f * m0 + 0.5f * p1 - 0.125f * m1;

    EXPECT_VEC3_NEAR (curve.GetCurvePoint (0.5f), expected);
}

TEST (CubicHermite, TangentMatchesNumericalDerivative)
{
    CubicHermite<Vector3f> curve (Vector3f (0, 0, 0), Vector3f (4, 2, 0), Vector3f (1, 3, 0), Vector3f (0, 1, 2));

    for (float t : { 0.2f, 0.5f, 0.8f })
    {
        EXPECT_VEC3_NEAR (curve.GetCurveTangent (t), NumericalTangent (curve, t), 1e-2f);
    }
}

TEST (CubicHermite, ZeroTangentsGiveSmoothEaseBetweenPoints)
{
    CubicHermite<Vector3f> curve (Vector3f (0, 0, 0), Vector3f (1, 0, 0), Vector3f (0, 0, 0), Vector3f (0, 0, 0));

    // with zero end tangents x(t) = 3t^2 - 2t^3 (smoothstep)
    EXPECT_NEAR (curve.GetCurvePoint (0.5f)[0], 0.5f, 1e-5f);
    EXPECT_NEAR (curve.GetCurvePoint (0.25f)[0], 0.15625f, 1e-5f);
}

TEST (CubicHermite, SetPointsAndTangentsReconfiguresCurve)
{
    CubicHermite<Vector3f> curve;
    curve.SetPointsAndTangents (Vector3f (1, 1, 1), Vector3f (2, 2, 2), Vector3f (0, 0, 0), Vector3f (0, 0, 0));

    EXPECT_VEC3_NEAR (curve.GetCurvePoint (0.0f), Vector3f (1, 1, 1));
    EXPECT_VEC3_NEAR (curve.GetCurvePoint (1.0f), Vector3f (2, 2, 2));
}
