#include "math/Vector4f.h"

// constructors
Vector4f::Vector4f () : v { 0.0f, 0.0f, 0.0f, 0.0f } {};
Vector4f::Vector4f (float x, float y, float z, float w) : v { x, y, z, w } {};
Vector4f::Vector4f (float x, float y, float z) : v { x, y, z, 1.0f } {};
Vector4f::Vector4f (const Vector3f& v3, float w) : v { v3[0], v3[1], v3[2], w } {};

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