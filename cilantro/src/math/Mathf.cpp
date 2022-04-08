#include "cilantroengine.h"
#include "math/Vector2f.h"
#include "math/Vector3f.h"
#include "math/Vector4f.h"
#include "math/Matrix3f.h"
#include "math/Matrix4f.h"
#include "math/Mathf.h"
#include "math/GaussLegendreIntegrator.h"

#include <cmath>
#include <vector>

// template instantiations
template void Mathf::SolveSystemOfLinearEquations<float> (std::vector<std::vector<float>>& A, std::vector<float>& b);
template void Mathf::SolveSystemOfLinearEquations<Vector3f> (std::vector<std::vector<float>>& A, std::vector<Vector3f>& b);
template void Mathf::SolveSystemOfLinearEquations<Vector4f> (std::vector<std::vector<float>>& A, std::vector<Vector4f>& b);

GaussLegendreIntegrator<INTEGRATOR_DEGREE> Mathf::integrator = GaussLegendreIntegrator<INTEGRATOR_DEGREE> ();

bool Mathf::VeryClose (const float a, const float b, int ulp = 2)
{
    return std::fabs (a - b) <= std::numeric_limits<float>::epsilon () * std::fabs (a + b) * ulp || std::fabs (a - b) < std::numeric_limits<float>::min ();
}

float Mathf::Length (const Vector3f & v)
{
    return std::sqrt (v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
}

float Mathf::Length (const Vector4f & v)
{
    return std::sqrt (v[0] * v[0] + v[1] * v[1] + v[2] * v[2] + v[3] * v[3]);
}

Vector3f Mathf::Normalize (const Vector3f& v)
{
    return v * (1.0f / Length (v));
}

Vector4f Mathf::Normalize (const Vector4f& v)
{
    return v * (1.0f / Length (v));
}

float Mathf::Dot (const Vector3f& v1, const Vector3f& v2)
{
    return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
}

Vector3f Mathf::Cross (const Vector3f& v1, const Vector3f& v2)
{
    return Vector3f (v1[1] * v2[2] - v1[2] * v2[1], v1[2] * v2[0] - v1[0] * v2[2], v1[0] * v2[1] - v1[1] * v2[0]);
}

Vector4f Mathf::CartesianToHomogenous (const Vector3f& v, float w)
{
    return Vector4f (v * w, w);
}

std::vector<Vector4f> Mathf::CartesianToHomogenous (const std::vector<Vector3f>& v, const std::vector<float>& w)
{
    std::vector<Vector4f> hv;

    for (unsigned int i = 0; i < v.size(); i++)
    {
        hv.push_back (Mathf::CartesianToHomogenous (v[i], w[i]));
    }

    return hv;
}

Vector3f Mathf::HomogenousToCartesianPerspective (const Vector4f& v)
{
    return Vector3f (v * (1.0f / v[3]));
}

Vector3f Mathf::HomogenousToCartesianTruncate (const Vector4f& v)
{
    return Vector3f (v);
}

float Mathf::GetHomogenousWeight (const Vector4f& v)
{
    return v[3];
}

float Mathf::Norm (const Quaternion& q)
{
    return std::sqrt (q.s * q.s + q.v[0] * q.v[0] + q.v[1] * q.v[1] + q.v[2] * q.v[2]);
}

float Mathf::Dot (const Quaternion& q1, const Quaternion& q2)
{
    return q1.s * q2.s + Mathf::Dot (q1.v, q2.v);
}

Quaternion Mathf::Normalize (const Quaternion& q)
{
    float norm = Mathf::Norm (q);

    return (1.0f / norm) * q;
}

Quaternion Mathf::Conjugate (const Quaternion& q)
{
    return Quaternion (q.s, -q.v);
}

Quaternion Mathf::Invert (const Quaternion& q)
{
    float norm = Mathf::Norm (q);
    float normSquared = norm * norm;

    return (1.0f / normSquared) * Mathf::Conjugate (q);
}

Quaternion Mathf::Product (const Quaternion& q, const Quaternion& r)
{
    return Quaternion (q.s * r.s - Mathf::Dot (q.v, r.v), q.s * r.v + r.s * q.v + Mathf::Cross (q.v, r.v));
}

Quaternion Mathf::GenRotationQuaternion (const Vector3f& axis, float theta)
{
    Vector3f axisNormalized = Mathf::Normalize (axis);
    Quaternion q (std::cos (theta * 0.5f), axisNormalized);

    q.v *= std::sin (theta * 0.5f);

    return q;
}

Vector3f Mathf::Rotate (const Vector3f& v, const Quaternion& q)
{
    Quaternion p (0.0f, v);
    Quaternion qInv = Mathf::Invert (q);
    Quaternion rotated = Mathf::Product (Mathf::Product (q, p), qInv);

    return rotated.v;
}

Vector3f Mathf::Rotate (const Vector3f& v, const Vector3f& axis, float theta)
{
    return Mathf::Rotate (v, Mathf::GenRotationQuaternion (axis, theta));
}

Vector3f Mathf::QuaternionToEuler (const Quaternion& q)
{
    Vector3f euler;
    float pole;

    pole = q.v[0] * q.v[1] + q.v[2] * q.s;

    // pitch
    euler[0] = std::atan2(2.0f * (q.s * q.v[0] - q.v[1] * q.v[2]), 1.0f - 2.0f * (q.v[0] * q.v[0] + q.v[2] * q.v[2]));

    // yaw
    euler[1] = std::atan2 (2.0f * (q.s * q.v[1] - q.v[0] * q.v[2]), 1.0f - 2.0f * (q.v[1] * q.v[1] + q.v[2] * q.v[2]));

    // roll
    euler[2] = std::asin (2.0f * (q.v[0] * q.v[1] + q.s * q.v[2]));

    if (pole > 0.499f)
    {
        // north pole        
        euler[0] = Mathf::Pi () * 0.5f;
        euler[1] = 2.0f * std::atan2 (q.v[1], q.s);
        euler[2] = 0.0f;
    }
    else if (pole < -0.499f)
    {
        // south pole
        euler[0] = -Mathf::Pi () * 0.5f;
        euler[1] = -2.0f * std::atan2 (q.v[1], q.s);
        euler[2] = 0.0f;		
    }

    return euler;
}

Quaternion Mathf::EulerToQuaterion (const Vector3f& euler)
{
    Quaternion q;

    // pitch (X)
    float cp = std::cos (euler[0] * 0.5f);
    float sp = std::sin (euler[0] * 0.5f);

    // yaw (Y)
    float cy = std::cos (euler[1] * 0.5f);
    float sy = std::sin (euler[1] * 0.5f);

    // roll (Z)
    float cr = std::cos (euler[2] * 0.5f);
    float sr = std::sin (euler[2] * 0.5f);

    // yaw, then pitch, then roll
    q.s = cy * cp * cr + sy * sp * sr;
    q.v[0] = cy * sp * cr + sy * cp * sr;
    q.v[1] = sy * cp * cr - cy * sp * sr;
    q.v[2] = cy * cp * sr - sy * sp * cr;

    return q;
}

float Mathf::Clamp (float vt, float v0, float v1)
{
    if (vt > v1)
    {
        return v1;
    }
    else if (vt < v0)
    {
        return v0;
    }
    return vt;
}

float Mathf::Step (float v0, float v1, float vt)
{
    return Mathf::Clamp ((vt - v0) / (v1 - v0), 0.0f, 1.0f);
}

float Mathf::Smoothstep (float v0, float v1, float vt)
{
    float x = Mathf::Step (v0, v1, vt);
    return Mathf::Clamp (x * x * (3.0f - 2.0f * x), 0.0f, 1.0f);
}

float Mathf::Smootherstep (float v0, float v1, float vt)
{
    float x = Mathf::Step (v0, v1, vt);

    return Mathf::Clamp (x * x * x * (x * (x * 6.0f - 15.0f) + 10.0f), 0.0f, 1.0f);
}

float Mathf::Lerp (float v0, float v1, float t)
{
    t = Mathf::Clamp (t, 0.0f, 1.0f);
    return (1.0f - t) * v0 + t * v1;
}

Vector3f Mathf::Lerp (const Vector3f& v0, const Vector3f& v1, float t)
{
    t = Mathf::Clamp (t, 0.0f, 1.0f);
    return (1.0f - t) * v0 + t * v1;
}

Quaternion Mathf::Lerp (const Quaternion& q0, const Quaternion& q1, float t)
{
    t = Mathf::Clamp (t, 0.0f, 1.0f);
    return (1.0f - t) * q0 + t * q1;
}

Quaternion Mathf::Slerp (const Quaternion& q0, const Quaternion& q1, float t)
{
    Quaternion result;
    Quaternion q0n = Mathf::Normalize (q0);
    Quaternion q1n = Mathf::Normalize (q1);

    t = Mathf::Clamp (t, 0.0f, 1.0f);
    float cos_theta = Mathf::Dot (q0n, q1n);

    if (cos_theta < 0.0f)
    {
        q1n = -q1n;
        cos_theta = -cos_theta;
    }

    if (cos_theta > 0.995f)
    {
        result = (1.0f - t) * q0n + t * q1n;
        return Mathf::Normalize (result);
    }

    float theta = std::acos (cos_theta);
    float theta_t = theta * t;
    float sin_theta = std::sin (theta);
    float sin_theta_t = std::sin (theta_t);

    float s0 = std::cos (theta_t) - cos_theta * sin_theta_t / sin_theta;
    float s1 = sin_theta_t / sin_theta;

    result = s0 * q0n + s1 * q1n;
    return Mathf::Normalize (result);
}


float Mathf::Integral (float a, float b, std::function<float(float)> f)
{
    return integrator.Integral (a, b, f);
}

float Mathf::Det (const Matrix3f & m)
{
    return m[0][0] * m[1][1] * m[2][2]
         - m[0][1] * m[1][0] * m[2][2]
         - m[0][0] * m[1][2] * m[2][1]
         + m[0][2] * m[1][0] * m[2][1]
         + m[0][1] * m[1][2] * m[2][0]
         - m[0][2] * m[1][1] * m[2][0];
}

float Mathf::Det (const Matrix4f & m)
{
    float Cof00 = + m[1][1] * (m[2][2] * m[3][3] - m[2][3] * m[3][2]) - m[1][2] * (m[2][1] * m[3][3] - m[2][3] * m[3][1]) + m[1][3] * (m[2][1] * m[3][2] - m[2][2] * m[3][1]);
    float Cof01 = - m[1][0] * (m[2][2] * m[3][3] - m[2][3] * m[3][2]) + m[1][2] * (m[2][0] * m[3][3] - m[2][3] * m[3][0]) - m[1][3] * (m[2][0] * m[3][2] - m[2][2] * m[3][0]);
    float Cof02 = + m[1][0] * (m[2][1] * m[3][3] - m[2][3] * m[3][1]) - m[1][1] * (m[2][0] * m[3][3] - m[2][3] * m[3][0]) + m[1][3] * (m[2][0] * m[3][1] - m[2][1] * m[3][0]);
    float Cof03 = - m[1][0] * (m[2][1] * m[3][2] - m[2][2] * m[3][1]) + m[1][1] * (m[2][0] * m[3][2] - m[2][2] * m[3][0]) - m[1][2] * (m[2][0] * m[3][1] - m[2][1] * m[3][0]);

    return m[0][0] * Cof00 + m[0][1] * Cof01 + m[0][2] * Cof02 + m[0][3] * Cof03;
}

Matrix3f Mathf::Transpose (const Matrix3f & m)
{
    Matrix3f n;

    n[0][0] = m[0][0];
    n[0][1] = m[1][0];
    n[0][2] = m[2][0];

    n[1][0] = m[0][1];
    n[1][1] = m[1][1];
    n[1][2] = m[2][1];

    n[2][0] = m[0][2];
    n[2][1] = m[1][2];
    n[2][2] = m[2][2];

    return n;
}

Matrix4f Mathf::Transpose (const Matrix4f & m)
{
    Matrix4f n;

    n[0][0] = m[0][0];
    n[0][1] = m[1][0];
    n[0][2] = m[2][0];
    n[0][3] = m[3][0];

    n[1][0] = m[0][1];
    n[1][1] = m[1][1];
    n[1][2] = m[2][1];
    n[1][3] = m[3][1];

    n[2][0] = m[0][2];
    n[2][1] = m[1][2];
    n[2][2] = m[2][2];
    n[2][3] = m[3][2];

    n[3][0] = m[0][3];
    n[3][1] = m[1][3];
    n[3][2] = m[2][3];
    n[3][3] = m[3][3];

    return n;
}

Matrix3f Mathf::Invert (const Matrix3f & m)
{
    Matrix3f i;

    i[0][0] = m[1][1] * m[2][2] - m[1][2] * m[2][1];
    i[0][1] = m[0][2] * m[2][1] - m[0][1] * m[2][2];
    i[0][2] = m[0][1] * m[1][2] - m[0][2] * m[1][1];

    i[1][0] = m[1][2] * m[2][0] - m[1][0] * m[2][2];
    i[1][1] = m[0][0] * m[2][2] - m[0][2] * m[2][0];
    i[1][2] = m[0][2] * m[1][0] - m[0][0] * m[1][2];

    i[2][0] = m[1][0] * m[2][1] - m[1][1] * m[2][0];
    i[2][1] = m[0][1] * m[2][0] - m[0][0] * m[2][1];
    i[2][2] = m[0][0] * m[1][1] - m[0][1] * m[1][0];

    i *= (1.0f / Det (m));

    return i;
}

Matrix4f Mathf::Invert (const Matrix4f & m)
{
    float a = m[2][2] * m[3][3] - m[2][3] * m[3][2];
    float b = m[2][1] * m[3][3] - m[2][3] * m[3][1];
    float c = m[2][1] * m[3][2] - m[2][2] * m[3][1];
    float d = m[2][0] * m[3][3] - m[2][3] * m[3][0];
    float e = m[2][0] * m[3][2] - m[2][2] * m[3][0];
    float f = m[2][0] * m[3][1] - m[2][1] * m[3][0];

    float g = m[1][2] * m[3][3] - m[1][3] * m[3][2];
    float h = m[1][1] * m[3][3] - m[1][3] * m[3][1];
    float i = m[1][1] * m[3][2] - m[1][2] * m[3][1];
    float j = m[1][0] * m[3][3] - m[1][3] * m[3][0];
    float k = m[1][0] * m[3][2] - m[1][2] * m[3][0];
    float l = m[1][0] * m[3][1] - m[1][1] * m[3][0];

    float n = m[1][2] * m[2][3] - m[1][3] * m[2][2];
    float o = m[1][1] * m[2][3] - m[1][3] * m[2][1];
    float p = m[1][1] * m[2][2] - m[1][2] * m[2][1];
    float q = m[1][0] * m[2][3] - m[1][3] * m[2][0];
    float r = m[1][0] * m[2][2] - m[1][2] * m[2][0];
    float s = m[1][0] * m[2][1] - m[1][1] * m[2][0];

    float Cof00 = + m[1][1] * a - m[1][2] * b + m[1][3] * c;
    float Cof01 = - m[1][0] * a + m[1][2] * d - m[1][3] * e;
    float Cof02 = + m[1][0] * b - m[1][1] * d + m[1][3] * f;
    float Cof03 = - m[1][0] * c + m[1][1] * e - m[1][2] * f;

    float Cof10 = - m[0][1] * a + m[0][2] * b - m[0][3] * c;
    float Cof11 = + m[0][0] * a - m[0][2] * d + m[0][3] * e;
    float Cof12 = - m[0][0] * b + m[0][1] * d - m[0][3] * f;
    float Cof13 = + m[0][0] * c - m[0][1] * e + m[0][2] * f;

    float Cof20 = + m[0][1] * g - m[0][2] * h + m[0][3] * i;
    float Cof21 = - m[0][0] * g + m[0][2] * j - m[0][3] * k;
    float Cof22 = + m[0][0] * h - m[0][1] * j + m[0][3] * l;
    float Cof23 = - m[0][0] * i + m[0][1] * k - m[0][2] * l;

    float Cof30 = - m[0][1] * n + m[0][2] * o - m[0][3] * p;
    float Cof31 = + m[0][0] * n - m[0][2] * q + m[0][3] * r;
    float Cof32 = - m[0][0] * o + m[0][1] * q - m[0][3] * s;
    float Cof33 = + m[0][0] * p - m[0][1] * r + m[0][2] * s;

    Matrix4f z (Vector4f (Cof00, Cof01, Cof02, Cof03), Vector4f (Cof10, Cof11, Cof12, Cof13), Vector4f (Cof20, Cof21, Cof22, Cof23), Vector4f (Cof30, Cof31, Cof32, Cof33));

    z *= 1.0f / (m[0][0] * Cof00 + m[0][1] * Cof01 + m[0][2] * Cof02 + m[0][3] * Cof03);

    return z;
}

unsigned int Mathf::Binomial (unsigned int n, unsigned int k)
{
    unsigned int c = 1;
    unsigned int i;

    if (k > n - k)
    {
        k = n - k;
    }

    for (i = 1; i <= k; i++)
    {
        if (c / i > std::numeric_limits<unsigned int>::max() / n)
        {
            // return 0 on overflow
            return 0;
        }
        c = c / i * n + c % i * n / i;
        --n;
    }

    return c;
}

float Mathf::Deg2Rad (float degrees)
{
    return degrees * Mathf::Pi() / 180.f;
}

Vector3f Mathf::Deg2Rad (const Vector3f& degrees)
{
    Vector3f radians;

    radians[0] = Mathf::Deg2Rad (degrees[0]);
    radians[1] = Mathf::Deg2Rad (degrees[1]);
    radians[2] = Mathf::Deg2Rad (degrees[2]);

    return radians;
}

float Mathf::Rad2Deg (float radians)
{
    return radians * 180.f / Mathf::Pi();
}

Vector3f Mathf::Rad2Deg (const Vector3f& radians)
{
    Vector3f degrees;

    degrees[0] = Mathf::Rad2Deg (radians[0]);
    degrees[1] = Mathf::Rad2Deg (radians[1]);
    degrees[2] = Mathf::Rad2Deg (radians[2]);

    return degrees;
}

Vector3f Mathf::Spherical2Cartesian (float theta, float phi, float r)
{
    return Vector3f (r * std::sin (theta) * std::cos (phi), r * std::cos (theta), r * std::sin (theta) * std::sin (phi));
}

Vector2f Mathf::Spherical2Cartesian (float theta, float r)
{
    return Vector2f (r * std::cos (theta), r * std::sin (theta));
}

Matrix4f Mathf::GenRotationXMatrix(float angle)
{
    Matrix4f m;

    m.InitIdentity ();
    m[1][1] = std::cos (angle);
    m[1][2] = -std::sin (angle);
    m[2][1] = std::sin (angle);
    m[2][2] = std::cos (angle);
    
    return m;
}

Matrix4f Mathf::GenRotationYMatrix (float angle)
{
    Matrix4f m;

    m.InitIdentity ();
    m[0][0] = std::cos (angle);
    m[0][2] = std::sin (angle);
    m[2][0] = -std::sin (angle);
    m[2][2] = std::cos (angle);

    return m;
}

Matrix4f Mathf::GenRotationZMatrix (float angle)
{
    Matrix4f m;

    m.InitIdentity ();
    m[0][0] = std::cos (angle);
    m[0][1] = -std::sin (angle);
    m[1][0] = std::sin (angle);
    m[1][1] = std::cos (angle);

    return m;
}

Matrix4f Mathf::GenRotationXYZMatrix (float x, float y, float z)
{
    return GenRotationZMatrix (z) * GenRotationYMatrix (y) * GenRotationXMatrix (x);
}

Matrix4f Mathf::GenRotationXYZMatrix (const Vector3f& r)
{
    return GenRotationXYZMatrix (r[0], r[1], r[2]);
}

Matrix4f Mathf::GenRotationMatrix (const Quaternion &q)
{
    Matrix4f m;
    Quaternion qn = Mathf::Normalize (q);

    m.InitIdentity ();

    float qx = q.v[0];
    float qy = q.v[1];
    float qz = q.v[2];
    float sqx = qx * qx;
    float sqy = qy * qy;
    float sqz = qz * qz;

    m[0][0] = 1.0f - 2.0f * sqy - 2.0f * sqz;
    m[0][1] = 2.0f * qx * qy - 2.0f * qz * q.s;
    m[0][2] = 2.0f * qx * qz + 2.0f * qy * q.s;

    m[1][0] = 2.0f * qx * qy + 2.0f * qz * q.s;
    m[1][1] = 1.0f - 2.0f * sqx - 2.0f * sqz;
    m[1][2] = 2.0f * qy * qz - 2.0f * qx * q.s;

    m[2][0] = 2.0f * qx * qz - 2.0f * qy * q.s;
    m[2][1] = 2.0f * qy * qz + 2.0f * qx * q.s;
    m[2][2] = 1.0f - 2.0f * sqx - 2.0f * sqy;

    return m;
}

Quaternion Mathf::GenQuaternion (const Matrix4f& m)
{
    Quaternion q;

    // matrix must represent pure rotation

    float trace = m[0][0] + m[1][1] + m[2][2];

    if (trace > 0)
    {
        float s = 0.5f / std::sqrt (1.0f + trace);
        q.s = 0.25f / s;
        q.v = Vector3f (m[2][1] - m[1][2], m[0][2] - m[2][0], m[1][0] - m[0][1]) * s;
    }
    else
    {
        if (m[0][0] > m[1][1] && m[0][0] > m[2][2] ) 
        {
            float s = 2.0f * std::sqrt (1.0f + m[0][0] - m[1][1] - m[2][2]);
            q.s = (m[2][1] - m[1][2]) / s;
            q.v[0] = 0.25f * s;
            q.v[1] = (m[0][1] + m[1][0]) / s;
            q.v[2] = (m[0][2] + m[2][0]) / s;
        } 
        else if (m[1][1] > m[2][2]) 
        {
            float s = 2.0f * std::sqrt (1.0f + m[1][1] - m[0][0] - m[2][2]);
            q.s = (m[0][2] - m[2][0]) / s;
            q.v[0] = (m[0][1] + m[1][0]) / s;
            q.v[1] = 0.25f * s;
            q.v[2] = (m[1][2] + m[2][1]) / s;
        } 
        else 
        {
            float s = 2.0f * std::sqrt (1.0f + m[2][2] - m[0][0] - m[1][1]);
            q.s = (m[1][0] - m[0][1]) / s;
            q.v[0] = (m[0][2] + m[2][0]) / s;
            q.v[1] = (m[1][2] + m[2][1]) / s;
            q.v[2] = 0.25f * s;
        }
    }

    return q;
}

Matrix4f Mathf::GenTranslationMatrix (float x, float y, float z)
{
    Matrix4f m;

    m.InitIdentity ();
    m[0][3] = x;
    m[1][3] = y;
    m[2][3] = z;

    return m;
}

Matrix4f Mathf::GenTranslationMatrix (const Vector3f & t)
{
    return Mathf::GenTranslationMatrix (t[0], t[1], t[2]);
}

Matrix4f Mathf::GenScalingMatrix (float x, float y, float z)
{
    Matrix4f m;

    m.InitIdentity ();
    m[0][0] = x;
    m[1][1] = y;
    m[2][2] = z;

    return m;
}

Matrix4f Mathf::GenScalingMatrix (const Vector3f & s)
{
    return GenScalingMatrix (s[0], s[1], s[2]);
}

Vector3f Mathf::GetScalingFromTransformationMatrix (const Matrix4f& m)
{
    Vector3f v;

    v[0] = Length (Vector3f (m[0][0], m[1][0], m[2][0]));
    v[1] = Length (Vector3f (m[0][1], m[1][1], m[2][1]));
    v[2] = Length (Vector3f (m[0][2], m[1][2], m[2][2]));

    return v;
}

Vector3f Mathf::GetTranslationFromTransformationMatrix (const Matrix4f& m)
{
    Vector3f v;

    v[0] = m[0][3];
    v[1] = m[1][3];
    v[2] = m[2][3];

    return v;
}

Quaternion Mathf::GetRotationFromTransformationMatrix (const Matrix4f& m)
{
    Matrix4f pureRotation;
    Vector3f v = GetScalingFromTransformationMatrix (m);

    pureRotation.InitIdentity ();

    pureRotation[0][0] = m[0][0] * v[0];
    pureRotation[1][0] = m[1][0] * v[0];
    pureRotation[2][0] = m[2][0] * v[0];

    pureRotation[0][1] = m[0][1] * v[1];
    pureRotation[1][1] = m[1][1] * v[1];
    pureRotation[2][1] = m[2][1] * v[1];

    pureRotation[0][2] = m[0][1] * v[2];
    pureRotation[1][2] = m[1][1] * v[2];
    pureRotation[2][2] = m[2][1] * v[2];

    return GenQuaternion (pureRotation);
}

Matrix4f Mathf::GenCameraViewMatrix (const Vector3f& position, const Vector3f& lookAt, const Vector3f& up)
{
    Vector3f u, v, n;

    // Create orthonormal basis
    n = Normalize (position - lookAt);
    u = Normalize (Cross (up, n));
    v = Cross (n, u);
    
    // Create a basis conversion matrix
    // It is an orthonormal matrix so inverse is equal to transposition
    Matrix4f m (Vector4f (u, 0.0f), Vector4f (v, 0.0f), Vector4f (n, 0.0f), Vector4f (0.0f, 0.0f, 0.0f, 1.0f));
    m = Transpose (m);
    
    // Create a translation matrix
    // The eye position is negated which is equivalent
    // to the inverse of translation matrix (so no need to invert)
    Matrix4f t = Mathf::GenTranslationMatrix (-position);

    // multiply matrices and return
    return m * t;
}

Quaternion Mathf::GenCameraOrientationQuaternion (const Vector3f& position, const Vector3f& lookAt, const Vector3f& up)
{
    Vector3f u, v, n;

    // Create orthonormal basis
    n = Normalize (position - lookAt);
    u = Normalize (Cross (up, n));
    v = Cross (n, u);
    
    // Create a basis conversion matrix
    // It is an orthonormal matrix so inverse is equal to transposition
    Matrix4f m (Vector4f (u, 0.0f), Vector4f (v, 0.0f), Vector4f (n, 0.0f), Vector4f (0.0f, 0.0f, 0.0f, 1.0f));

    // m represents pure rotation so it may be converted to quaternion
    return Mathf::GenQuaternion (m);
}

Matrix4f Mathf::GenPerspectiveProjectionMatrix (float aspect, float fov, float nearZ, float farZ)
{
    Matrix4f m;
    float scale = std::tan (fov * 0.5f);

    m.InitIdentity ();
    m[0][0] = 1.0f / (aspect * scale);
    m[1][1] = 1.0f / scale;
    m[2][2] = -(farZ + nearZ) / (farZ - nearZ);
    m[2][3] = -(2.0f * nearZ * farZ) / (farZ - nearZ);
    m[3][2] = -1.0f;

    return m;
}

Matrix4f Mathf::GenOrthographicProjectionMatrix (float aspect, float width, float nearZ, float farZ)
{
    Matrix4f m;

    m.InitIdentity ();
    m[0][0] = 2.0f / width;
    m[1][1] = (2.0f * aspect) / width;
    m[2][2] = -2.0f / (farZ - nearZ);
    m[2][3] = -(farZ + nearZ) / (farZ - nearZ);

    return m;
}

Matrix4f Mathf::GenOrthographicProjectionMatrix (float left, float right, float top, float bottom, float near, float far)
{
    Matrix4f m;

    m.InitIdentity ();
    m[0][0] = 2.0f / (right - left);
    m[1][1] = 2.0f / (top - bottom);
    m[2][2] = -2.0f / (far - near);
    m[0][3] = -(right + left) / (right - left);
    m[1][3] = -(top + bottom) / (top - bottom);
    m[2][3] = -(far + near) / (far - near);

    return m;
}

template <typename T>
void Mathf::SolveSystemOfLinearEquations (std::vector<std::vector<float>>& A, std::vector<T>& b)
{
    float max_abs;
    int max_abs_row;
    int m = b.size ();
    int n;

    // check matrix and vector sizes
    if ((A.size () != m) || (m == 0))
    {
        return;
    }
    else
    {
        n = A[0].size ();
    }

    for (auto&& row : A)
    {
        if (row.size() != n)
        {
            return;
        }
    }

    for (int j = 0; j < m; j++)
    {    
        // find pivot in current column
        max_abs = std::abs (A[j][j]);
        max_abs_row = j;
        for (int i = j + 1; i < n; i++)
        {
            if (std::abs (A[i][j]) > max_abs)
            {
                max_abs = std::abs (A[i][j]);
                max_abs_row = i;
            }
        }

        // proceed only if pivot found
        if (max_abs != 0)
        {
            // swap rows
            std::swap (A[j], A[max_abs_row]);
            std::swap (b[j], b[max_abs_row]);

            // lower part
            for (int i = j + 1; i < n; i++)
            {
                float s = -A[i][j] / max_abs;
                A[i][j] = 0;
                for (int k = j + 1; k < m; k++)
                {
                    A[i][k] = A[i][k] + A[j][k] * s;
                }
                b[i] = b[i] + b[j] * s;
            }

            // upper part
            for (int i = j - 1; i >= 0; i--)
            {
                float s = -A[i][j] / max_abs;
                A[i][j] = 0;
                for (int k = j + 1; k < m; k++)
                {
                    A[i][k] = A[i][k] + A[j][k] * s;
                }
                b[i] = b[i] + b[j] * s;
            }

            // diagonal
            float s = max_abs;
            A[j][j] = 1;
            for (int k = j + 1; k < m; k++)
            {
                   A[j][k] = A[j][k] / s;
            }
            b[j] = b[j] * (1.0f / s);

        }

    }

   
}
