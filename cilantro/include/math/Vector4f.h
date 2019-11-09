#ifndef _VECTOR4F_H_
#define _VECTOR4F_H_

#include "cilantroengine.h"
#include "math/Vector3f.h"
#include <algorithm>

// Represents 4-dimensional float vector
class Vector4f
{
public:
	// constructors
	__EAPI Vector4f ();
    __EAPI Vector4f (float x, float y, float z, float w);
    __EAPI Vector4f (float x, float y, float z);
    __EAPI Vector4f (const Vector3f& v3, float w);

    // copy constructor
    __EAPI Vector4f (const Vector4f& other);

    // move constructor
    __EAPI Vector4f (Vector4f&& other);

    // assignment operator
    __EAPI Vector4f& operator= (Vector4f other);

    // destructor
    __EAPI ~Vector4f ();

    // accessor and mutator
	__EAPI float& operator[] (unsigned int index);
	__EAPI const float& operator[] (unsigned int index) const;

private:
	float v[4];

};

#endif

