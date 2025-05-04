#include "cilantroengine.h"
#include "math/Matrix4f.h"
#include <initializer_list>

#define M4(i, j) m[i * 4 + j]

namespace cilantro {

// constructor
Matrix4f::Matrix4f () { };

// initializer list constructor
Matrix4f::Matrix4f (std::initializer_list<float> initializer)
{
    float* mPtr = m;

    for (auto&& i : initializer)
    {
        *mPtr++ = i;
    }
}

// copy constructor
Matrix4f::Matrix4f (const Matrix4f& other)
{
    std::copy (other.m, other.m + 16, m);
}

// copy constructor (column vectors)
Matrix4f::Matrix4f (const Vector4f& c1, const Vector4f& c2, const Vector4f& c3, const Vector4f& c4)
{
    M4 (0, 0) = c1[0];
    M4 (1, 0) = c1[1];
    M4 (2, 0) = c1[2];
    M4 (3, 0) = c1[3];

    M4 (0, 1) = c2[0];
    M4 (1, 1) = c2[1];
    M4 (2, 1) = c2[2];
    M4 (3, 1) = c2[3];

    M4 (0, 2) = c3[0];
    M4 (1, 2) = c3[1];
    M4 (2, 2) = c3[2];
    M4 (3, 2) = c3[3];

    M4 (0, 3) = c4[0];
    M4 (1, 3) = c4[1];
    M4 (2, 3) = c4[2];
    M4 (3, 3) = c4[3];
}

// move constructor
Matrix4f::Matrix4f (Matrix4f&& other) : Matrix4f ()
{
    std::swap (m, other.m);
}

// array constructor
Matrix4f::Matrix4f (const float* m)
{
    std::copy (m, m + 16, this->m);
}

// assignment operator
Matrix4f& Matrix4f::operator= (Matrix4f other)
{
    std::swap (m, other.m);
    return *this;
}

// destructor
Matrix4f::~Matrix4f () {};

Matrix4f& Matrix4f::InitIdentity()
{ 
    M4 (0, 0) = 1.0f;
    M4 (0, 1) = 0.0f;
    M4 (0, 2) = 0.0f;
    M4 (0, 3) = 0.0f;
    
    M4 (1, 0) = 0.0f;
    M4 (1, 1) = 1.0f;
    M4 (1, 2) = 0.0f;
    M4 (1, 3) = 0.0f;
    
    M4 (2, 0) = 0.0f;
    M4 (2, 1) = 0.0f;
    M4 (2, 2) = 1.0f;
    M4 (2, 3) = 0.0f;

    M4 (3, 0) = 0.0f;
    M4 (3, 1) = 0.0f;
    M4 (3, 2) = 0.0f;
    M4 (3, 3) = 1.0f;

    return *this;
}

// mutator
float* Matrix4f::operator[] (unsigned int index)
{
    return m + index * 4;
}

// accessor
const float* Matrix4f::operator[] (unsigned int index) const
{
    return m + index * 4;
}

// compound assignment operator for matrix multiplication
Matrix4f& Matrix4f::operator*=(const Matrix4f& other)
{
    // actual multiplication code
    Matrix4f temp (*this);

    M4 (0, 0) = temp[0][0] * other[0][0] + temp[0][1] * other[1][0] + temp[0][2] * other[2][0] + temp[0][3] * other[3][0];
    M4 (0, 1) = temp[0][0] * other[0][1] + temp[0][1] * other[1][1] + temp[0][2] * other[2][1] + temp[0][3] * other[3][1];
    M4 (0, 2) = temp[0][0] * other[0][2] + temp[0][1] * other[1][2] + temp[0][2] * other[2][2] + temp[0][3] * other[3][2];
    M4 (0, 3) = temp[0][0] * other[0][3] + temp[0][1] * other[1][3] + temp[0][2] * other[2][3] + temp[0][3] * other[3][3];

    M4 (1, 0) = temp[1][0] * other[0][0] + temp[1][1] * other[1][0] + temp[1][2] * other[2][0] + temp[1][3] * other[3][0];
    M4 (1, 1) = temp[1][0] * other[0][1] + temp[1][1] * other[1][1] + temp[1][2] * other[2][1] + temp[1][3] * other[3][1];
    M4 (1, 2) = temp[1][0] * other[0][2] + temp[1][1] * other[1][2] + temp[1][2] * other[2][2] + temp[1][3] * other[3][2];
    M4 (1, 3) = temp[1][0] * other[0][3] + temp[1][1] * other[1][3] + temp[1][2] * other[2][3] + temp[1][3] * other[3][3];

    M4 (2, 0) = temp[2][0] * other[0][0] + temp[2][1] * other[1][0] + temp[2][2] * other[2][0] + temp[2][3] * other[3][0];
    M4 (2, 1) = temp[2][0] * other[0][1] + temp[2][1] * other[1][1] + temp[2][2] * other[2][1] + temp[2][3] * other[3][1];
    M4 (2, 2) = temp[2][0] * other[0][2] + temp[2][1] * other[1][2] + temp[2][2] * other[2][2] + temp[2][3] * other[3][2];
    M4 (2, 3) = temp[2][0] * other[0][3] + temp[2][1] * other[1][3] + temp[2][2] * other[2][3] + temp[2][3] * other[3][3];

    M4 (3, 0) = temp[3][0] * other[0][0] + temp[3][1] * other[1][0] + temp[3][2] * other[2][0] + temp[3][3] * other[3][0];
    M4 (3, 1) = temp[3][0] * other[0][1] + temp[3][1] * other[1][1] + temp[3][2] * other[2][1] + temp[3][3] * other[3][1];
    M4 (3, 2) = temp[3][0] * other[0][2] + temp[3][1] * other[1][2] + temp[3][2] * other[2][2] + temp[3][3] * other[3][2];
    M4 (3, 3) = temp[3][0] * other[0][3] + temp[3][1] * other[1][3] + temp[3][2] * other[2][3] + temp[3][3] * other[3][3];

    return *this;
}

// matrix by vector pre-multiplication 
Vector4f Matrix4f::operator*(const Vector4f& v) const
{
    Vector4f temp;

    temp[0] = M4 (0, 0) * v[0] + M4 (0, 1) * v[1] + M4 (0, 2) * v[2] + M4 (0, 3) * v[3];
    temp[1] = M4 (1, 0) * v[0] + M4 (1, 1) * v[1] + M4 (1, 2) * v[2] + M4 (1, 3) * v[3];
    temp[2] = M4 (2, 0) * v[0] + M4 (2, 1) * v[1] + M4 (2, 2) * v[2] + M4 (2, 3) * v[3];
    temp[3] = M4 (3, 0) * v[0] + M4 (3, 1) * v[1] + M4 (3, 2) * v[2] + M4 (3, 3) * v[3];

    return temp;
}

Matrix4f& Matrix4f::operator*=(float f)
{
    M4 (0, 0) *= f;
    M4 (0, 1) *= f;
    M4 (0, 2) *= f;
    M4 (0, 3) *= f;

    M4 (1, 0) *= f;
    M4 (1, 1) *= f;
    M4 (1, 2) *= f;
    M4 (1, 3) *= f;

    M4 (2, 0) *= f;
    M4 (2, 1) *= f;
    M4 (2, 2) *= f;
    M4 (2, 3) *= f;

    M4 (3, 0) *= f;
    M4 (3, 1) *= f;
    M4 (3, 2) *= f;
    M4 (3, 3) *= f;

    return *this;
}

// binary operator for matrix by matrix multiplication
Matrix4f operator* (Matrix4f m, const Matrix4f& n)
{
    m *= n;
    return m;
}

// binary operator for matrix by float multiplication
Matrix4f operator* (Matrix4f m, float f)
{
    m *= f;
    return m;
}

Matrix4f operator* (float f, Matrix4f m)
{
    m *= f;
    return m;
}

} // namespace cilantro
