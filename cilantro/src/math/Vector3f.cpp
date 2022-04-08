#include "math/Vector3f.h"
#include "math/Vector4f.h"
#include <initializer_list>
    
// constructors
Vector3f::Vector3f () : v { 0.0f, 0.0f, 0.0f } { };
Vector3f::Vector3f (float x, float y, float z) : v { x, y, z } { };
Vector3f::Vector3f (const Vector4f& v4) : v { v4[0], v4[1], v4[2]} {};

// initializer list constructor
Vector3f::Vector3f (std::initializer_list<float> initializer)
{
    float* vPtr = v;

    for (auto&& i : initializer)
    {
        *vPtr++ = i;
    }
}

// copy constructor
Vector3f::Vector3f (const Vector3f& other)
{
    std::copy (other.v, other.v + 3, v);
}

// move constructor
Vector3f::Vector3f (Vector3f&& other) : Vector3f ()
{
    std::swap (v, other.v);
}

// assignment operator
Vector3f& Vector3f::operator= (Vector3f other)
{
    std::swap (v, other.v);
    return *this;
}

// destructor
Vector3f::~Vector3f () { };

// vector dimension
unsigned int Vector3f::Dim () const
{
    return 3u;
}

// mutator
float& Vector3f::operator[](unsigned int index)
{
    return v[index];
}

// accessor
const float& Vector3f::operator[](unsigned int index) const
{
    return v[index];
}

Vector3f& Vector3f::operator*=(float f)
{
    v[0] *= f;
    v[1] *= f;
    v[2] *= f;

    return *this;
}

Vector3f& Vector3f::operator/=(float f)
{
    v[0] /= f;
    v[1] /= f;
    v[2] /= f;

    return *this;
}

Vector3f& Vector3f::operator+=(const Vector3f& other)
{
    v[0] += other.v[0];
    v[1] += other.v[1];
    v[2] += other.v[2];

    return *this;
}

Vector3f& Vector3f::operator-=(const Vector3f& other)
{
    v[0] -= other.v[0];
    v[1] -= other.v[1];
    v[2] -= other.v[2];

    return *this;
}

bool Vector3f::operator== (const Vector3f& other) const
{
    return (v[0] == other[0]) && (v[1] == other[1]) && (v[2] == other[2]);
}

Vector3f operator* (Vector3f u, float f)
{
    u *= f;
    return u;
}

Vector3f operator* (float f, Vector3f u)
{
    u *= f;
    return u;
}

Vector3f operator/ (Vector3f u, float f)
{
    u /= f;
    return u;
}

Vector3f operator+ (Vector3f u, const Vector3f& v)
{
    u += v;
    return u;
}

Vector3f operator- (Vector3f u, const Vector3f& v)
{
    u -= v;
    return u;
}

Vector3f operator- (Vector3f v)
{
    v.v[0] = -v.v[0];
    v.v[1] = -v.v[1];
    v.v[2] = -v.v[2];

    return v;
}
