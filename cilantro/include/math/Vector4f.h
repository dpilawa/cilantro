#ifndef _VECTOR4F_H_
#define _VECTOR4F_H_

#include "cilantroengine.h"
#include "math/Vector3f.h"
#include <algorithm>
#include <initializer_list>

// Represents 4-dimensional float vector
class Vector4f
{
public:
	// constructors
	__EAPI Vector4f ();
    __EAPI Vector4f (float x, float y, float z, float w);
    __EAPI Vector4f (float x, float y, float z);
    __EAPI Vector4f (const Vector3f& v3, float w);

	// initializer list constructor
    __EAPI Vector4f (std::initializer_list<float> i);

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

	// operators
	__EAPI Vector4f& operator*= (float f);
	__EAPI Vector4f& operator+= (const Vector4f& other);
	__EAPI Vector4f& operator-= (const Vector4f& other);
	
	__EAPI friend Vector4f operator- (Vector4f v);

private:
	float v[4];

};

__EAPI Vector4f operator* (Vector4f u, float f);
__EAPI Vector4f operator* (float f, Vector4f u);
__EAPI Vector4f operator+ (Vector4f u, const Vector4f& v);
__EAPI Vector4f operator- (Vector4f u, const Vector4f& v);
__EAPI Vector4f operator- (Vector4f v);

#endif

