#include "math/Vector4f.h"
#include <initializer_list>

// constructors
Vector4f::Vector4f () : v { 0.0f, 0.0f, 0.0f, 0.0f } {};
Vector4f::Vector4f (float x, float y, float z, float w) : v { x, y, z, w } {};
Vector4f::Vector4f (float x, float y, float z) : v { x, y, z, 1.0f } {};
Vector4f::Vector4f (const Vector3f& v3, float w) : v { v3[0], v3[1], v3[2], w } {};

// initializer list constructor
Vector4f::Vector4f (std::initializer_list<float> initializer)
{
    float* vPtr = v;

    for (auto&& i : initializer)
    {
        *vPtr++ = i;
    }
}

// copy constructor
Vector4f::Vector4f (const Vector4f& other)
{
    std::copy (other.v, other.v + 4, v);
}

// move constructor
Vector4f::Vector4f (Vector4f&& other) : Vector4f()
{
    std::swap (v, other.v);
}

// assignment operator
Vector4f& Vector4f::operator= (Vector4f other)
{
    std::swap (v, other.v);
    return *this;
}

// destructor
Vector4f::~Vector4f () { };

// vector dimenison
unsigned int Vector4f::Dim () const
{
    return 4u;
}

// mutator
float& Vector4f::operator[] (unsigned int index)
{
    return v[index];
}

// accessor
const float& Vector4f::operator[] (unsigned int index) const
{
    return v[index];
}


Vector4f& Vector4f::operator*=(float f)
{
    v[0] *= f;
    v[1] *= f;
    v[2] *= f;
    v[3] *= f;

    return *this;
}

Vector4f& Vector4f::operator+=(const Vector4f& other)
{
    v[0] += other.v[0];
    v[1] += other.v[1];
    v[2] += other.v[2];
    v[3] += other.v[3];

    return *this;
}

Vector4f& Vector4f::operator-=(const Vector4f& other)
{
    v[0] -= other.v[0];
    v[1] -= other.v[1];
    v[2] -= other.v[2];
    v[3] -= other.v[3];

    return *this;
}

Vector4f operator* (Vector4f u, float f)
{
    u *= f;
    return u;
}

Vector4f operator* (float f, Vector4f u)
{
    u *= f;
    return u;
}

Vector4f operator+ (Vector4f u, const Vector4f& v)
{
    u += v;
    return u;
}

Vector4f operator- (Vector4f u, const Vector4f& v)
{
    u -= v;
    return u;
}

Vector4f operator- (Vector4f v)
{
    v.v[0] = -v.v[0];
    v.v[1] = -v.v[1];
    v.v[2] = -v.v[2];
    v.v[3] = -v.v[3];

    return v;
}
