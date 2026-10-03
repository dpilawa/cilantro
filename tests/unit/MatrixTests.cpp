#include "TestHelpers.h"

using namespace cilantro;
using namespace cilantro::testing;

namespace {

const float kPi = Mathf::Pi ();

Matrix4f Trs (const Vector3f& translation, const Vector3f& eulerRadians, const Vector3f& scale)
{
    return Mathf::GenTranslationMatrix (translation) * Mathf::GenRotationXYZMatrix (eulerRadians) * Mathf::GenScalingMatrix (scale);
}

} // namespace

// ---------------------------------------------------------------------------
// Matrix4f
// ---------------------------------------------------------------------------

TEST (Matrix4f, InitIdentity)
{
    Matrix4f m;
    m.InitIdentity ();

    for (unsigned int i = 0; i < 4; i++)
    {
        for (unsigned int j = 0; j < 4; j++)
        {
            EXPECT_FLOAT_EQ (m[i][j], i == j ? 1.0f : 0.0f);
        }
    }
}

TEST (Matrix4f, InitializerListIsRowMajor)
{
    Matrix4f m = { 1, 2, 3, 4,
                   5, 6, 7, 8,
                   9, 10, 11, 12,
                   13, 14, 15, 16 };

    EXPECT_FLOAT_EQ (m[0][0], 1.0f);
    EXPECT_FLOAT_EQ (m[0][3], 4.0f);
    EXPECT_FLOAT_EQ (m[1][2], 7.0f);
    EXPECT_FLOAT_EQ (m[3][0], 13.0f);
    EXPECT_FLOAT_EQ (m[3][3], 16.0f);
}

TEST (Matrix4f, ColumnVectorConstructorFillsColumns)
{
    Matrix4f m (Vector4f (1, 2, 3, 4), Vector4f (5, 6, 7, 8), Vector4f (9, 10, 11, 12), Vector4f (13, 14, 15, 16));

    // first argument is the first column
    EXPECT_FLOAT_EQ (m[0][0], 1.0f);
    EXPECT_FLOAT_EQ (m[1][0], 2.0f);
    EXPECT_FLOAT_EQ (m[3][0], 4.0f);
    EXPECT_FLOAT_EQ (m[0][1], 5.0f);
    EXPECT_FLOAT_EQ (m[0][3], 13.0f);
    EXPECT_FLOAT_EQ (m[3][3], 16.0f);
}

TEST (Matrix4f, ArrayConstructorCopiesData)
{
    float data[16];
    for (int i = 0; i < 16; i++)
    {
        data[i] = static_cast<float>(i);
    }

    Matrix4f m (data);

    EXPECT_FLOAT_EQ (m[0][1], 1.0f);
    EXPECT_FLOAT_EQ (m[2][3], 11.0f);
}

TEST (Matrix4f, IdentityIsNeutralForMultiplication)
{
    Matrix4f m = { 1, 2, 3, 4,
                   0, 1, 4, 5,
                   2, 0, 1, 6,
                   0, 0, 0, 1 };

    EXPECT_MAT4_NEAR (m * Identity4 (), m);
    EXPECT_MAT4_NEAR (Identity4 () * m, m);
}

TEST (Matrix4f, MultiplicationIsAssociativeButNotCommutative)
{
    Matrix4f a = Mathf::GenRotationXMatrix (0.3f);
    Matrix4f b = Mathf::GenTranslationMatrix (1.0f, 2.0f, 3.0f);
    Matrix4f c = Mathf::GenScalingMatrix (2.0f, 3.0f, 4.0f);

    EXPECT_MAT4_NEAR ((a * b) * c, a * (b * c));

    // translation after rotation differs from rotation after translation
    Matrix4f ab = a * b;
    Matrix4f ba = b * a;
    EXPECT_NE (ab[1][3], ba[1][3]);
}

TEST (Matrix4f, MultiplyByVectorUsesColumnVectorConvention)
{
    Matrix4f t = Mathf::GenTranslationMatrix (1.0f, 2.0f, 3.0f);

    EXPECT_VEC4_NEAR (t * Vector4f (1.0f, 1.0f, 1.0f, 1.0f), Vector4f (2.0f, 3.0f, 4.0f, 1.0f));

    // directions (w = 0) are not translated
    EXPECT_VEC4_NEAR (t * Vector4f (1.0f, 1.0f, 1.0f, 0.0f), Vector4f (1.0f, 1.0f, 1.0f, 0.0f));
}

TEST (Matrix4f, ScalarMultiplication)
{
    Matrix4f m = Identity4 ();
    Matrix4f scaled = m * 3.0f;
    Matrix4f scaled2 = 3.0f * m;

    EXPECT_FLOAT_EQ (scaled[0][0], 3.0f);
    EXPECT_FLOAT_EQ (scaled[3][3], 3.0f);
    EXPECT_FLOAT_EQ (scaled[0][1], 0.0f);
    EXPECT_MAT4_NEAR (scaled, scaled2);
}

TEST (Matrix4f, CopyAndMovePreserveContents)
{
    Matrix4f a = Mathf::GenTranslationMatrix (1.0f, 2.0f, 3.0f);
    Matrix4f copy (a);
    Matrix4f moved (std::move (copy));
    Matrix4f assigned;
    assigned = a;

    EXPECT_MAT4_NEAR (moved, a);
    EXPECT_MAT4_NEAR (assigned, a);
}

// ---------------------------------------------------------------------------
// Matrix3f
// ---------------------------------------------------------------------------

TEST (Matrix3f, InitializerListIsRowMajor)
{
    Matrix3f m = { 1, 2, 3,
                   4, 5, 6,
                   7, 8, 9 };

    EXPECT_FLOAT_EQ (m[0][2], 3.0f);
    EXPECT_FLOAT_EQ (m[1][0], 4.0f);
    EXPECT_FLOAT_EQ (m[2][1], 8.0f);
}

TEST (Matrix3f, SubmatrixOfMatrix4fIsUpperLeftBlock)
{
    Matrix4f m4 = { 1, 2, 3, 4,
                    5, 6, 7, 8,
                    9, 10, 11, 12,
                    13, 14, 15, 16 };
    Matrix3f m3 (m4);

    EXPECT_FLOAT_EQ (m3[0][0], 1.0f);
    EXPECT_FLOAT_EQ (m3[0][2], 3.0f);
    EXPECT_FLOAT_EQ (m3[1][1], 6.0f);
    EXPECT_FLOAT_EQ (m3[2][2], 11.0f);
}

TEST (Matrix3f, ColumnVectorConstructorFillsColumns)
{
    Matrix3f m (Vector3f (1, 2, 3), Vector3f (4, 5, 6), Vector3f (7, 8, 9));

    EXPECT_FLOAT_EQ (m[0][0], 1.0f);
    EXPECT_FLOAT_EQ (m[2][0], 3.0f);
    EXPECT_FLOAT_EQ (m[0][1], 4.0f);
    EXPECT_FLOAT_EQ (m[2][2], 9.0f);
}

TEST (Matrix3f, MultiplicationAndIdentity)
{
    Matrix3f a = { 1, 2, 0,
                   0, 1, 3,
                   4, 0, 1 };
    Matrix3f b = { 2, 0, 1,
                   1, 1, 0,
                   0, 5, 1 };

    EXPECT_MAT3_NEAR (a * Identity3 (), a);
    EXPECT_MAT3_NEAR (Identity3 () * a, a);

    Matrix3f ab = a * b;
    // first row of a * b: [1 2 0] * b = [2+2, 0+2, 1+0]
    EXPECT_FLOAT_EQ (ab[0][0], 4.0f);
    EXPECT_FLOAT_EQ (ab[0][1], 2.0f);
    EXPECT_FLOAT_EQ (ab[0][2], 1.0f);
}

TEST (Matrix3f, MultiplyByVector)
{
    Matrix3f m = { 0, -1, 0,
                   1, 0, 0,
                   0, 0, 1 };

    EXPECT_VEC3_NEAR (m * Vector3f (1.0f, 0.0f, 0.0f), Vector3f (0.0f, 1.0f, 0.0f));
}

// ---------------------------------------------------------------------------
// Determinant, transpose, inverse
// ---------------------------------------------------------------------------

TEST (MathfMatrix, DeterminantOfKnownMatrices)
{
    EXPECT_NEAR (Mathf::Det (Identity3 ()), 1.0f, 1e-6f);
    EXPECT_NEAR (Mathf::Det (Identity4 ()), 1.0f, 1e-6f);
    EXPECT_NEAR (Mathf::Det (Mathf::GenScalingMatrix (2.0f, 3.0f, 4.0f)), 24.0f, 1e-4f);

    Matrix3f m = { 2, 0, 1,
                   1, 3, 2,
                   1, 1, 4 };
    // 2 * (3 * 4 - 2 * 1) - 0 + 1 * (1 * 1 - 3 * 1) = 20 - 2
    EXPECT_NEAR (Mathf::Det (m), 18.0f, 1e-4f);
}

TEST (MathfMatrix, RotationMatricesHaveUnitDeterminant)
{
    EXPECT_NEAR (Mathf::Det (Mathf::GenRotationXMatrix (0.7f)), 1.0f, 1e-5f);
    EXPECT_NEAR (Mathf::Det (Mathf::GenRotationYMatrix (-1.2f)), 1.0f, 1e-5f);
    EXPECT_NEAR (Mathf::Det (Mathf::GenRotationZMatrix (2.4f)), 1.0f, 1e-5f);
}

TEST (MathfMatrix, SingularMatrixHasZeroDeterminant)
{
    Matrix3f m = { 1, 2, 3,
                   2, 4, 6,
                   0, 1, 5 };

    EXPECT_NEAR (Mathf::Det (m), 0.0f, 1e-5f);
}

TEST (MathfMatrix, TransposeSwapsRowsAndColumnsAndIsInvolutive)
{
    Matrix4f m = { 1, 2, 3, 4,
                   5, 6, 7, 8,
                   9, 10, 11, 12,
                   13, 14, 15, 16 };
    Matrix4f t = Mathf::Transpose (m);

    EXPECT_FLOAT_EQ (t[0][1], 5.0f);
    EXPECT_FLOAT_EQ (t[1][0], 2.0f);
    EXPECT_FLOAT_EQ (t[3][2], 12.0f);
    EXPECT_MAT4_NEAR (Mathf::Transpose (t), m);

    Matrix3f m3 = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    EXPECT_MAT3_NEAR (Mathf::Transpose (Mathf::Transpose (m3)), m3);
    EXPECT_FLOAT_EQ (Mathf::Transpose (m3)[0][2], 7.0f);
}

TEST (MathfMatrix, InverseOfIdentityIsIdentity)
{
    EXPECT_MAT3_NEAR (Mathf::Invert (Identity3 ()), Identity3 ());
    EXPECT_MAT4_NEAR (Mathf::Invert (Identity4 ()), Identity4 ());
}

TEST (MathfMatrix, Matrix3fInverseMultipliesToIdentity)
{
    Matrix3f m = { 2, 0, 1,
                   1, 3, 2,
                   1, 1, 4 };

    EXPECT_MAT3_NEAR (m * Mathf::Invert (m), Identity3 ());
    EXPECT_MAT3_NEAR (Mathf::Invert (m) * m, Identity3 ());
}

TEST (MathfMatrix, Matrix4fInverseMultipliesToIdentity)
{
    Matrix4f m = Trs (Vector3f (1.0f, -2.0f, 3.0f), Vector3f (0.3f, -0.7f, 1.1f), Vector3f (2.0f, 0.5f, 3.0f));

    EXPECT_MAT4_NEAR (m * Mathf::Invert (m), Identity4 (), 1e-3f);
    EXPECT_MAT4_NEAR (Mathf::Invert (m) * m, Identity4 (), 1e-3f);
}

TEST (MathfMatrix, InverseOfRotationIsTranspose)
{
    Matrix4f r = Mathf::GenRotationXYZMatrix (0.4f, 0.9f, -0.2f);

    EXPECT_MAT4_NEAR (Mathf::Invert (r), Mathf::Transpose (r), 1e-4f);
}

// ---------------------------------------------------------------------------
// Transformation matrices
// ---------------------------------------------------------------------------

TEST (MathfTransform, TranslationMatrixPutsOffsetInLastColumn)
{
    Matrix4f t = Mathf::GenTranslationMatrix (Vector3f (1.0f, 2.0f, 3.0f));

    EXPECT_FLOAT_EQ (t[0][3], 1.0f);
    EXPECT_FLOAT_EQ (t[1][3], 2.0f);
    EXPECT_FLOAT_EQ (t[2][3], 3.0f);
    EXPECT_MAT4_NEAR (t, Mathf::GenTranslationMatrix (1.0f, 2.0f, 3.0f));
}

TEST (MathfTransform, ScalingMatrixScalesAxes)
{
    Matrix4f s = Mathf::GenScalingMatrix (Vector3f (2.0f, 3.0f, 4.0f));

    EXPECT_VEC4_NEAR (s * Vector4f (1.0f, 1.0f, 1.0f, 1.0f), Vector4f (2.0f, 3.0f, 4.0f, 1.0f));
    EXPECT_MAT4_NEAR (s, Mathf::GenScalingMatrix (2.0f, 3.0f, 4.0f));
}

TEST (MathfTransform, AxisRotationsAreRightHanded)
{
    const float quarter = kPi * 0.5f;

    // X axis: y -> z
    EXPECT_VEC4_NEAR (Mathf::GenRotationXMatrix (quarter) * Vector4f (0, 1, 0, 1), Vector4f (0, 0, 1, 1));
    // Y axis: z -> x
    EXPECT_VEC4_NEAR (Mathf::GenRotationYMatrix (quarter) * Vector4f (0, 0, 1, 1), Vector4f (1, 0, 0, 1));
    // Z axis: x -> y
    EXPECT_VEC4_NEAR (Mathf::GenRotationZMatrix (quarter) * Vector4f (1, 0, 0, 1), Vector4f (0, 1, 0, 1));
}

TEST (MathfTransform, RotationXYZAppliesXThenYThenZ)
{
    const float quarter = kPi * 0.5f;
    Matrix4f r = Mathf::GenRotationXYZMatrix (quarter, quarter, quarter);

    Vector4f expected = Mathf::GenRotationZMatrix (quarter) * (Mathf::GenRotationYMatrix (quarter) * (Mathf::GenRotationXMatrix (quarter) * Vector4f (1, 2, 3, 1)));

    EXPECT_VEC4_NEAR (r * Vector4f (1, 2, 3, 1), expected);
    EXPECT_MAT4_NEAR (r, Mathf::GenRotationXYZMatrix (Vector3f (quarter, quarter, quarter)));
}

TEST (MathfTransform, RotationPreservesLengthOfVectors)
{
    Matrix4f r = Mathf::GenRotationXYZMatrix (0.4f, -1.3f, 2.2f);
    Vector4f v = r * Vector4f (1.0f, 2.0f, 3.0f, 0.0f);

    EXPECT_NEAR (Mathf::Length (Vector3f (v)), Mathf::Length (Vector3f (1.0f, 2.0f, 3.0f)), 1e-4f);
}

TEST (MathfTransform, ScalingAndTranslationAreExtractedFromTrsMatrix)
{
    Matrix4f m = Trs (Vector3f (1.0f, -2.0f, 3.0f), Vector3f (0.3f, -0.7f, 1.1f), Vector3f (2.0f, 0.5f, 3.0f));

    EXPECT_VEC3_NEAR (Mathf::GetTranslationFromTransformationMatrix (m), Vector3f (1.0f, -2.0f, 3.0f));
    EXPECT_VEC3_NEAR (Mathf::GetScalingFromTransformationMatrix (m), Vector3f (2.0f, 0.5f, 3.0f));
}

// Known defect: GetRotationFromTransformationMatrix multiplies columns by the scale instead of dividing,
// and copies the third column from m[..][1] instead of m[..][2], so the extracted rotation is wrong
// (even for a pure rotation). Enable once fixed.
TEST (MathfTransform, DISABLED_RotationIsExtractedFromPureRotationMatrix)
{
    Quaternion expected = Mathf::GenRotationQuaternion (Vector3f (0.0f, 0.0f, 1.0f), kPi * 0.5f);
    Quaternion actual = Mathf::GetRotationFromTransformationMatrix (Mathf::GenRotationZMatrix (kPi * 0.5f));

    EXPECT_SAME_ROTATION (actual, expected);
}

TEST (MathfTransform, DISABLED_RotationIsExtractedFromScaledMatrix)
{
    Matrix4f m = Trs (Vector3f (1.0f, 2.0f, 3.0f), Vector3f (0.0f, 0.0f, kPi * 0.5f), Vector3f (2.0f, 2.0f, 2.0f));
    Quaternion expected = Mathf::GenRotationQuaternion (Vector3f (0.0f, 0.0f, 1.0f), kPi * 0.5f);

    EXPECT_SAME_ROTATION (Mathf::GetRotationFromTransformationMatrix (m), expected);
}

// ---------------------------------------------------------------------------
// Camera and projection
// ---------------------------------------------------------------------------

TEST (MathfCamera, ViewMatrixOfDefaultCameraIsIdentity)
{
    Matrix4f view = Mathf::GenCameraViewMatrix (Vector3f (0, 0, 0), Vector3f (0, 0, -1), Vector3f (0, 1, 0));

    EXPECT_MAT4_NEAR (view, Identity4 ());
}

TEST (MathfCamera, ViewMatrixMovesEyeToOrigin)
{
    Vector3f eye (3.0f, 4.0f, 5.0f);
    Matrix4f view = Mathf::GenCameraViewMatrix (eye, Vector3f (0, 0, 0), Vector3f (0, 1, 0));

    EXPECT_VEC4_NEAR (view * Vector4f (eye, 1.0f), Vector4f (0, 0, 0, 1));
}

TEST (MathfCamera, ViewMatrixPutsLookAtPointOnNegativeZ)
{
    Vector3f eye (0.0f, 0.0f, 5.0f);
    Matrix4f view = Mathf::GenCameraViewMatrix (eye, Vector3f (0, 0, 0), Vector3f (0, 1, 0));

    // the look-at point is 5 units in front of the camera, i.e. along -Z in view space
    EXPECT_VEC4_NEAR (view * Vector4f (0, 0, 0, 1), Vector4f (0, 0, -5, 1));
}

TEST (MathfCamera, ViewMatrixIsRigid)
{
    Matrix4f view = Mathf::GenCameraViewMatrix (Vector3f (2, 3, 4), Vector3f (-1, 0, 1), Vector3f (0, 1, 0));

    EXPECT_NEAR (Mathf::Det (Matrix3f (view)), 1.0f, 1e-4f);
}

TEST (MathfCamera, PerspectiveMapsNearAndFarPlanesToNdcRange)
{
    const float nearZ = 1.0f;
    const float farZ = 10.0f;
    Matrix4f p = Mathf::GenPerspectiveProjectionMatrix (1.0f, kPi * 0.5f, nearZ, farZ);

    Vector4f nearPoint = p * Vector4f (0, 0, -nearZ, 1);
    Vector4f farPoint = p * Vector4f (0, 0, -farZ, 1);

    EXPECT_NEAR (nearPoint[2] / nearPoint[3], -1.0f, 1e-4f);
    EXPECT_NEAR (farPoint[2] / farPoint[3], 1.0f, 1e-4f);
}

TEST (MathfCamera, PerspectiveMapsFrustumEdgeToNdcEdge)
{
    // fov 90 degrees and aspect 1: at distance d the visible half-extent is d
    Matrix4f p = Mathf::GenPerspectiveProjectionMatrix (1.0f, kPi * 0.5f, 1.0f, 10.0f);
    Vector4f edge = p * Vector4f (4.0f, 4.0f, -4.0f, 1.0f);

    EXPECT_NEAR (edge[0] / edge[3], 1.0f, 1e-4f);
    EXPECT_NEAR (edge[1] / edge[3], 1.0f, 1e-4f);
}

TEST (MathfCamera, PerspectiveAspectRatioNarrowsHorizontalExtent)
{
    Matrix4f p = Mathf::GenPerspectiveProjectionMatrix (2.0f, kPi * 0.5f, 1.0f, 10.0f);
    Vector4f point = p * Vector4f (4.0f, 0.0f, -4.0f, 1.0f);

    EXPECT_NEAR (point[0] / point[3], 0.5f, 1e-4f);
}

TEST (MathfCamera, OrthographicMapsBoxCornersToNdcCorners)
{
    Matrix4f o = Mathf::GenOrthographicProjectionMatrix (-4.0f, 4.0f, 3.0f, -3.0f, 1.0f, 11.0f);

    // left, bottom, near corner and right, top, far corner (camera looks along -Z)
    EXPECT_VEC4_NEAR (o * Vector4f (-4.0f, -3.0f, -1.0f, 1.0f), Vector4f (-1, -1, -1, 1));
    EXPECT_VEC4_NEAR (o * Vector4f (4.0f, 3.0f, -11.0f, 1.0f), Vector4f (1, 1, 1, 1));
    EXPECT_VEC4_NEAR (o * Vector4f (0.0f, 0.0f, -6.0f, 1.0f), Vector4f (0, 0, 0, 1));
}
