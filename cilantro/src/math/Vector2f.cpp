#include "math/Vector2f.h"
#include <initializer_list>
    
// constructors
Vector2f::Vector2f () : v { 0.0f, 0.0f } { };
Vector2f::Vector2f (float x, float y) : v { x, y } { };

// initializer list constructor
Vector2f::Vector2f (std::initializer_list<float> initializer)
{
    float* vPtr = v;

    for (auto&& i : initializer)
    {
        *vPtr++ = i;
    }
}

// copy constructor
Vector2f::Vector2f (const Vector2f& other)
{
    std::copy (other.v, other.v + 2, v);
}

// move constructor
Vector2f::Vector2f (Vector2f&& other) : Vector2f ()
{
    std::swap (v, other.v);
}

// assignment operator
Vector2f& Vector2f::operator= (Vector2f other)
{
    std::swap (v, other.v);
    return *this;
}

// destructor
Vector2f::~Vector2f () { };

// vector dimension
unsigned int Vector2f::Dim () const
{
    return 2u;
}

// mutator
float& Vector2f::operator[](unsigned int index)
{
    return v[index];
}

// accessor
const float& Vector2f::operator[](unsigned int index) const
{
    return v[index];
}

Vector2f& Vector2f::operator*=(float f)
{
    v[0] *= f;
    v[1] *= f;

    return *this;
}

Vector2f& Vector2f::operator/=(float f)
{
    v[0] /= f;
    v[1] /= f;

    return *this;
}

Vector2f& Vector2f::operator+=(const Vector2f& other)
{
    v[0] += other.v[0];
    v[1] += other.v[1];

    return *this;
}

Vector2f& Vector2f::operator-=(const Vector2f& other)
{
    v[0] -= other.v[0];
    v[1] -= other.v[1];

    return *this;
}

bool Vector2f::operator== (const Vector2f& other) const
{
    return (v[0] == other[0]) && (v[1] == other[1]);
}

Vector2f operator* (Vector2f u, float f)
{
    u *= f;
    return u;
}

Vector2f operator* (float f, Vector2f u)
{
    u *= f;
    return u;
}

Vector2f operator/ (Vector2f u, float f)
{
    u /= f;
    return u;
}

Vector2f operator+ (Vector2f u, const Vector2f& v)
{
    u += v;
    return u;
}

Vector2f operator- (Vector2f u, const Vector2f& v)
{
    u -= v;
    return u;
}

Vector2f operator- (Vector2f v)
{
    v.v[0] = -v.v[0];
    v.v[1] = -v.v[1];

    return v;
}
