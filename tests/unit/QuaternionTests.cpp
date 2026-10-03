#include "TestHelpers.h"

using namespace cilantro;
using namespace cilantro::testing;

namespace {

const float kPi = Mathf::Pi ();
const Quaternion kIdentity (1.0f, 0.0f, 0.0f, 0.0f);

Quaternion AxisAngle (float x, float y, float z, float angle)
{
    return Mathf::GenRotationQuaternion (Vector3f (x, y, z), angle);
}

} // namespace

TEST (Quaternion, NormOfIdentityIsOne)
{
    EXPECT_NEAR (Mathf::Norm (kIdentity), 1.0f, 1e-6f);
    EXPECT_NEAR (Mathf::Norm (Quaternion (1.0f, 2.0f, 2.0f, 4.0f)), 5.0f, 1e-5f);
}

TEST (Quaternion, ConstructsFromScalarAndVector)
{
    Quaternion a (1.0f, 2.0f, 3.0f, 4.0f);
    Quaternion b (1.0f, Vector3f (2.0f, 3.0f, 4.0f));

    EXPECT_NEAR (Mathf::Dot (a, b), Mathf::Norm (a) * Mathf::Norm (b), 1e-4f);
}

TEST (Quaternion, NormalizeProducesUnitQuaternion)
{
    Quaternion q = Mathf::Normalize (Quaternion (2.0f, -1.0f, 3.0f, 0.5f));

    EXPECT_NEAR (Mathf::Norm (q), 1.0f, 1e-5f);
}

TEST (Quaternion, ArithmeticOperators)
{
    Quaternion a (1.0f, 2.0f, 3.0f, 4.0f);
    Quaternion b (4.0f, 3.0f, 2.0f, 1.0f);

    // sum of components checked through the dot product with the identity (scalar part)
    EXPECT_FLOAT_EQ (Mathf::Dot (a + b, kIdentity), 5.0f);
    EXPECT_FLOAT_EQ (Mathf::Dot (a - b, kIdentity), -3.0f);
    EXPECT_FLOAT_EQ (Mathf::Dot (2.0f * a, kIdentity), 2.0f);
    EXPECT_FLOAT_EQ (Mathf::Dot (a * 3.0f, kIdentity), 3.0f);
    EXPECT_FLOAT_EQ (Mathf::Dot (-a, kIdentity), -1.0f);
}

TEST (Quaternion, ConjugateNegatesVectorPart)
{
    Quaternion q (1.0f, 2.0f, 3.0f, 4.0f);
    Quaternion c = Mathf::Conjugate (q);

    // q * conjugate(q) = |q|^2 (pure scalar)
    Quaternion p = Mathf::Product (q, c);
    float n2 = Mathf::Norm (q) * Mathf::Norm (q);

    EXPECT_NEAR (Mathf::Dot (p, kIdentity), n2, 1e-4f);
    EXPECT_NEAR (Mathf::Norm (p), n2, 1e-4f);
}

TEST (Quaternion, InverseMultipliesToIdentity)
{
    Quaternion q (1.0f, 2.0f, 3.0f, 4.0f);
    Quaternion product = Mathf::Product (q, Mathf::Invert (q));

    EXPECT_SAME_ROTATION (product, kIdentity);
    EXPECT_NEAR (Mathf::Dot (product, kIdentity), 1.0f, 1e-5f);
}

TEST (Quaternion, ProductWithIdentityIsNeutral)
{
    Quaternion q = AxisAngle (1.0f, 2.0f, 3.0f, 0.8f);

    EXPECT_NEAR (Mathf::Dot (Mathf::Product (q, kIdentity), q), 1.0f, 1e-5f);
    EXPECT_NEAR (Mathf::Dot (Mathf::Product (kIdentity, q), q), 1.0f, 1e-5f);
}

TEST (QuaternionRotation, AxisAngleQuaternionHasUnitNorm)
{
    EXPECT_NEAR (Mathf::Norm (AxisAngle (1.0f, 2.0f, 3.0f, 1.234f)), 1.0f, 1e-5f);
    // the axis does not have to be normalized
    EXPECT_SAME_ROTATION (AxisAngle (0.0f, 0.0f, 5.0f, 0.5f), AxisAngle (0.0f, 0.0f, 1.0f, 0.5f));
}

TEST (QuaternionRotation, ZeroAngleIsIdentityRotation)
{
    Vector3f v (1.0f, 2.0f, 3.0f);

    EXPECT_VEC3_NEAR (Mathf::Rotate (v, AxisAngle (0.0f, 1.0f, 0.0f, 0.0f)), v);
}

TEST (QuaternionRotation, QuarterTurnsAroundAxes)
{
    const float quarter = kPi * 0.5f;

    EXPECT_VEC3_NEAR (Mathf::Rotate (Vector3f (1, 0, 0), AxisAngle (0, 0, 1, quarter)), Vector3f (0, 1, 0));
    EXPECT_VEC3_NEAR (Mathf::Rotate (Vector3f (0, 1, 0), AxisAngle (1, 0, 0, quarter)), Vector3f (0, 0, 1));
    EXPECT_VEC3_NEAR (Mathf::Rotate (Vector3f (0, 0, 1), AxisAngle (0, 1, 0, quarter)), Vector3f (1, 0, 0));
}

TEST (QuaternionRotation, RotateOverloadWithAxisAndAngle)
{
    Vector3f rotated = Mathf::Rotate (Vector3f (1, 0, 0), Vector3f (0, 0, 1), kPi);

    EXPECT_VEC3_NEAR (rotated, Vector3f (-1, 0, 0));
}

TEST (QuaternionRotation, RotationPreservesLength)
{
    Vector3f v (1.0f, -2.0f, 3.0f);
    Vector3f r = Mathf::Rotate (v, AxisAngle (1.0f, 1.0f, 0.0f, 2.1f));

    EXPECT_NEAR (Mathf::Length (r), Mathf::Length (v), 1e-4f);
}

TEST (QuaternionRotation, AxisIsFixedByRotationAroundIt)
{
    Vector3f axis = Mathf::Normalize (Vector3f (1.0f, 2.0f, 3.0f));

    EXPECT_VEC3_NEAR (Mathf::Rotate (axis, axis, 1.0f), axis);
}

TEST (QuaternionRotation, ProductComposesRotationsRightToLeft)
{
    const float quarter = kPi * 0.5f;
    Quaternion rx = AxisAngle (1, 0, 0, quarter);
    Quaternion rz = AxisAngle (0, 0, 1, quarter);
    Vector3f v (1.0f, 2.0f, 3.0f);

    // Product (a, b) applies b first, then a
    Vector3f composed = Mathf::Rotate (v, Mathf::Product (rz, rx));
    Vector3f sequential = Mathf::Rotate (Mathf::Rotate (v, rx), rz);

    EXPECT_VEC3_NEAR (composed, sequential);
}

TEST (QuaternionRotation, InverseUndoesRotation)
{
    Quaternion q = AxisAngle (1.0f, -2.0f, 0.5f, 1.3f);
    Vector3f v (3.0f, 1.0f, -2.0f);

    EXPECT_VEC3_NEAR (Mathf::Rotate (Mathf::Rotate (v, q), Mathf::Invert (q)), v);
}

// ---------------------------------------------------------------------------
// Quaternion <-> matrix
// ---------------------------------------------------------------------------

TEST (QuaternionMatrix, RotationMatrixAgreesWithQuaternionRotation)
{
    Quaternion q = AxisAngle (1.0f, 2.0f, -1.0f, 0.9f);
    Matrix4f m = Mathf::GenRotationMatrix (q);
    Vector3f v (1.0f, -2.0f, 0.5f);

    EXPECT_VEC3_NEAR (Vector3f (m * Vector4f (v, 0.0f)), Mathf::Rotate (v, q));
}

TEST (QuaternionMatrix, RotationMatrixOfIdentityIsIdentity)
{
    EXPECT_MAT4_NEAR (Mathf::GenRotationMatrix (kIdentity), Identity4 ());
}

TEST (QuaternionMatrix, RotationMatrixMatchesAxisMatrices)
{
    EXPECT_MAT4_NEAR (Mathf::GenRotationMatrix (AxisAngle (1, 0, 0, 0.6f)), Mathf::GenRotationXMatrix (0.6f));
    EXPECT_MAT4_NEAR (Mathf::GenRotationMatrix (AxisAngle (0, 1, 0, 0.6f)), Mathf::GenRotationYMatrix (0.6f));
    EXPECT_MAT4_NEAR (Mathf::GenRotationMatrix (AxisAngle (0, 0, 1, 0.6f)), Mathf::GenRotationZMatrix (0.6f));
}

TEST (QuaternionMatrix, RotationMatrixOfNonUnitQuaternionIsRotation)
{
    Quaternion unit = AxisAngle (0, 0, 1, 0.6f);
    Quaternion scaled = 3.0f * unit;

    EXPECT_MAT4_NEAR (Mathf::GenRotationMatrix (scaled), Mathf::GenRotationMatrix (unit));
}

TEST (QuaternionMatrix, QuaternionFromRotationMatrixRoundTrips)
{
    // covers the trace > 0 branch and all three largest-diagonal branches (half turns around each axis)
    const Quaternion cases[] = {
        AxisAngle (1.0f, 2.0f, 3.0f, 0.5f),
        AxisAngle (1.0f, 0.0f, 0.0f, kPi),
        AxisAngle (0.0f, 1.0f, 0.0f, kPi),
        AxisAngle (0.0f, 0.0f, 1.0f, kPi),
        AxisAngle (1.0f, 1.0f, 0.0f, 2.8f),
        AxisAngle (0.0f, 1.0f, 1.0f, 3.0f),
    };

    for (const Quaternion& q : cases)
    {
        Quaternion back = Mathf::GenQuaternion (Mathf::GenRotationMatrix (q));

        EXPECT_SAME_ROTATION (back, q);
    }
}

TEST (QuaternionMatrix, CameraOrientationQuaternionMatchesViewRotation)
{
    Vector3f eye (0, 0, 5);
    Vector3f lookAt (0, 0, 0);
    Vector3f up (0, 1, 0);

    // camera looking down -Z from +Z has no rotation
    Quaternion q = Mathf::GenCameraOrientationQuaternion (eye, lookAt, up);

    EXPECT_SAME_ROTATION (q, kIdentity);
}

TEST (QuaternionMatrix, CameraOrientationQuaternionRotatesForwardAxisToViewDirection)
{
    Vector3f eye (0, 0, 0);
    Vector3f lookAt (1, 0, 0);
    Quaternion q = Mathf::GenCameraOrientationQuaternion (eye, lookAt, Vector3f (0, 1, 0));

    // the camera's local -Z axis points to the look-at direction (+X)
    EXPECT_VEC3_NEAR (Mathf::Rotate (Vector3f (0, 0, -1), q), Vector3f (1, 0, 0));
}

// ---------------------------------------------------------------------------
// Interpolation
// ---------------------------------------------------------------------------

TEST (QuaternionInterpolation, SlerpEndpoints)
{
    Quaternion a = AxisAngle (0, 0, 1, 0.2f);
    Quaternion b = AxisAngle (0, 0, 1, 1.4f);

    EXPECT_SAME_ROTATION (Mathf::Slerp (a, b, 0.0f), a);
    EXPECT_SAME_ROTATION (Mathf::Slerp (a, b, 1.0f), b);
}

TEST (QuaternionInterpolation, SlerpMidpointIsHalfwayRotation)
{
    Quaternion a = AxisAngle (0, 0, 1, 0.0f);
    Quaternion b = AxisAngle (0, 0, 1, kPi * 0.5f);

    Quaternion mid = Mathf::Slerp (a, b, 0.5f);

    EXPECT_SAME_ROTATION (mid, AxisAngle (0, 0, 1, kPi * 0.25f));
}

TEST (QuaternionInterpolation, SlerpTakesShortestPath)
{
    Quaternion a = AxisAngle (0, 0, 1, 0.0f);
    Quaternion b = AxisAngle (0, 0, 1, 1.0f);

    // -b is the same rotation as b, interpolation must not go the long way round
    Quaternion mid = Mathf::Slerp (a, -b, 0.5f);

    EXPECT_SAME_ROTATION (mid, AxisAngle (0, 0, 1, 0.5f));
}

TEST (QuaternionInterpolation, SlerpResultIsUnitLengthAndClampsT)
{
    Quaternion a = AxisAngle (1, 0, 0, 0.3f);
    Quaternion b = AxisAngle (0, 1, 0, 1.1f);

    EXPECT_NEAR (Mathf::Norm (Mathf::Slerp (a, b, 0.37f)), 1.0f, 1e-5f);
    EXPECT_SAME_ROTATION (Mathf::Slerp (a, b, -2.0f), a);
    EXPECT_SAME_ROTATION (Mathf::Slerp (a, b, 5.0f), b);
}

TEST (QuaternionInterpolation, SlerpOfNearlyEqualQuaternionsIsStable)
{
    Quaternion a = AxisAngle (0, 1, 0, 0.5f);
    Quaternion b = AxisAngle (0, 1, 0, 0.5001f);

    Quaternion mid = Mathf::Slerp (a, b, 0.5f);

    EXPECT_NEAR (Mathf::Norm (mid), 1.0f, 1e-5f);
    EXPECT_SAME_ROTATION (mid, AxisAngle (0, 1, 0, 0.50005f));
}

TEST (QuaternionInterpolation, LerpEndpointsAndClamping)
{
    Quaternion a = AxisAngle (0, 0, 1, 0.2f);
    Quaternion b = AxisAngle (0, 0, 1, 0.8f);

    EXPECT_SAME_ROTATION (Mathf::Lerp (a, b, 0.0f), a);
    EXPECT_SAME_ROTATION (Mathf::Lerp (a, b, 1.0f), b);
    EXPECT_SAME_ROTATION (Mathf::Lerp (a, b, 2.0f), b);
}

// ---------------------------------------------------------------------------
// Euler angles
// ---------------------------------------------------------------------------

TEST (QuaternionEuler, ZeroEulerAnglesGiveIdentity)
{
    EXPECT_SAME_ROTATION (Mathf::EulerToQuaternion (Vector3f (0, 0, 0)), kIdentity);
    EXPECT_NEAR (Mathf::Norm (Mathf::EulerToQuaternion (Vector3f (0.3f, 0.5f, -0.2f))), 1.0f, 1e-5f);
}

TEST (QuaternionEuler, SingleAxisEulerAnglesMatchAxisRotations)
{
    EXPECT_SAME_ROTATION (Mathf::EulerToQuaternion (Vector3f (0.7f, 0, 0)), AxisAngle (1, 0, 0, 0.7f));
    EXPECT_SAME_ROTATION (Mathf::EulerToQuaternion (Vector3f (0, 0.7f, 0)), AxisAngle (0, 1, 0, 0.7f));
    EXPECT_SAME_ROTATION (Mathf::EulerToQuaternion (Vector3f (0, 0, 0.7f)), AxisAngle (0, 0, 1, 0.7f));
}

TEST (QuaternionEuler, SingleAxisAnglesRoundTrip)
{
    EXPECT_VEC3_NEAR (Mathf::QuaternionToEuler (Mathf::EulerToQuaternion (Vector3f (0.6f, 0, 0))), Vector3f (0.6f, 0, 0));
    EXPECT_VEC3_NEAR (Mathf::QuaternionToEuler (Mathf::EulerToQuaternion (Vector3f (0, 0.6f, 0))), Vector3f (0, 0.6f, 0));
    EXPECT_VEC3_NEAR (Mathf::QuaternionToEuler (Mathf::EulerToQuaternion (Vector3f (0, 0, 0.6f))), Vector3f (0, 0, 0.6f));
}

TEST (QuaternionEuler, GeneralAnglesRoundTrip)
{
    Vector3f euler (0.4f, -0.6f, 0.3f);

    EXPECT_VEC3_NEAR (Mathf::QuaternionToEuler (Mathf::EulerToQuaternion (euler)), euler, 1e-3f);
}

TEST (QuaternionEuler, EulerQuaternionComposesRollThenPitchThenYaw)
{
    // euler = (pitch around X, yaw around Y, roll around Z); the quaternion is Ry (yaw) * Rx (pitch) * Rz (roll),
    // i.e. roll is applied first. Note this differs from GenRotationXYZMatrix, which applies X first.
    Vector3f euler (0.5f, -0.4f, 0.3f);
    Vector3f v (1.0f, 2.0f, 3.0f);

    Matrix4f expected = Mathf::GenRotationYMatrix (euler[1]) * Mathf::GenRotationXMatrix (euler[0]) * Mathf::GenRotationZMatrix (euler[2]);

    EXPECT_VEC3_NEAR (Mathf::Rotate (v, Mathf::EulerToQuaternion (euler)), Vector3f (expected * Vector4f (v, 0.0f)));
}

TEST (QuaternionEuler, AnglesRoundTripOverTheValidRange)
{
    // pitch stays away from +/- 90 degrees (gimbal lock), yaw and roll cover the full circle
    for (float pitch = -1.4f; pitch <= 1.4f; pitch += 0.35f)
    {
        for (float yaw = -3.0f; yaw <= 3.0f; yaw += 0.75f)
        {
            for (float roll = -3.0f; roll <= 3.0f; roll += 0.75f)
            {
                Vector3f euler (pitch, yaw, roll);
                Vector3f back = Mathf::QuaternionToEuler (Mathf::EulerToQuaternion (euler));

                EXPECT_VEC3_NEAR (back, euler, 2e-3f);
            }
        }
    }
}

TEST (QuaternionEuler, ConvertedEulerAnglesRepresentTheSameRotationAtGimbalLock)
{
    // at +/- 90 degrees of pitch yaw and roll are interchangeable, so only the resulting rotation can be compared
    for (float pitch : { kPi * 0.5f, -kPi * 0.5f })
    {
        Quaternion original = Mathf::EulerToQuaternion (Vector3f (pitch, 0.7f, 0.2f));
        Vector3f euler = Mathf::QuaternionToEuler (original);

        EXPECT_NEAR (euler[0], pitch, 1e-3f);
        EXPECT_SAME_ROTATION (Mathf::EulerToQuaternion (euler), original, 2e-3f);
    }
}
