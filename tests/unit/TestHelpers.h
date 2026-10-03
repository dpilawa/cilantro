#pragma once

#include <gtest/gtest.h>
#include "math/Vector2f.h"
#include "math/Vector3f.h"
#include "math/Vector4f.h"
#include "math/Matrix3f.h"
#include "math/Matrix4f.h"
#include "math/Quaternion.h"
#include "math/Mathf.h"
#include <cmath>

namespace cilantro::testing {

constexpr float kTolerance = 1e-4f;

inline ::testing::AssertionResult Vec2Near (const Vector2f& a, const Vector2f& b, float tol = kTolerance)
{
    for (unsigned int i = 0; i < 2; i++)
    {
        if (std::fabs (a[i] - b[i]) > tol)
        {
            return ::testing::AssertionFailure () << "component " << i << " differs: (" << a[0] << ", " << a[1] << ") vs (" << b[0] << ", " << b[1] << ")";
        }
    }
    return ::testing::AssertionSuccess ();
}

inline ::testing::AssertionResult Vec3Near (const Vector3f& a, const Vector3f& b, float tol = kTolerance)
{
    for (unsigned int i = 0; i < 3; i++)
    {
        if (std::fabs (a[i] - b[i]) > tol)
        {
            return ::testing::AssertionFailure () << "component " << i << " differs: (" << a[0] << ", " << a[1] << ", " << a[2] << ") vs (" << b[0] << ", " << b[1] << ", " << b[2] << ")";
        }
    }
    return ::testing::AssertionSuccess ();
}

inline ::testing::AssertionResult Vec4Near (const Vector4f& a, const Vector4f& b, float tol = kTolerance)
{
    for (unsigned int i = 0; i < 4; i++)
    {
        if (std::fabs (a[i] - b[i]) > tol)
        {
            return ::testing::AssertionFailure () << "component " << i << " differs: (" << a[0] << ", " << a[1] << ", " << a[2] << ", " << a[3] << ") vs (" << b[0] << ", " << b[1] << ", " << b[2] << ", " << b[3] << ")";
        }
    }
    return ::testing::AssertionSuccess ();
}

inline ::testing::AssertionResult Mat3Near (const Matrix3f& a, const Matrix3f& b, float tol = kTolerance)
{
    for (unsigned int i = 0; i < 3; i++)
    {
        for (unsigned int j = 0; j < 3; j++)
        {
            if (std::fabs (a[i][j] - b[i][j]) > tol)
            {
                return ::testing::AssertionFailure () << "element [" << i << "][" << j << "] differs: " << a[i][j] << " vs " << b[i][j];
            }
        }
    }
    return ::testing::AssertionSuccess ();
}

inline ::testing::AssertionResult Mat4Near (const Matrix4f& a, const Matrix4f& b, float tol = kTolerance)
{
    for (unsigned int i = 0; i < 4; i++)
    {
        for (unsigned int j = 0; j < 4; j++)
        {
            if (std::fabs (a[i][j] - b[i][j]) > tol)
            {
                return ::testing::AssertionFailure () << "element [" << i << "][" << j << "] differs: " << a[i][j] << " vs " << b[i][j];
            }
        }
    }
    return ::testing::AssertionSuccess ();
}

// quaternions q and -q represent the same rotation, so compare using |dot| = |q1| * |q2|
inline ::testing::AssertionResult SameRotation (const Quaternion& a, const Quaternion& b, float tol = kTolerance)
{
    float expected = Mathf::Norm (a) * Mathf::Norm (b);
    float actual = std::fabs (Mathf::Dot (a, b));

    if (std::fabs (expected - actual) > tol)
    {
        return ::testing::AssertionFailure () << "quaternions represent different rotations: |dot| = " << actual << ", expected " << expected;
    }
    return ::testing::AssertionSuccess ();
}

inline Matrix4f Identity4 ()
{
    Matrix4f m;
    m.InitIdentity ();
    return m;
}

inline Matrix3f Identity3 ()
{
    Matrix3f m;
    m.InitIdentity ();
    return m;
}

} // namespace cilantro::testing

#define EXPECT_VEC2_NEAR(...) EXPECT_TRUE (::cilantro::testing::Vec2Near (__VA_ARGS__))
#define EXPECT_VEC3_NEAR(...) EXPECT_TRUE (::cilantro::testing::Vec3Near (__VA_ARGS__))
#define EXPECT_VEC4_NEAR(...) EXPECT_TRUE (::cilantro::testing::Vec4Near (__VA_ARGS__))
#define EXPECT_MAT3_NEAR(...) EXPECT_TRUE (::cilantro::testing::Mat3Near (__VA_ARGS__))
#define EXPECT_MAT4_NEAR(...) EXPECT_TRUE (::cilantro::testing::Mat4Near (__VA_ARGS__))
#define EXPECT_SAME_ROTATION(...) EXPECT_TRUE (::cilantro::testing::SameRotation (__VA_ARGS__))
