#ifndef _VECTOR3F_H_
#define _VECTOR3F_H_

#include "cilantroengine.h"
#include <algorithm>
#include <initializer_list>

namespace cilantro {

class Vector4f;

// Represents 3-dimensional float vector
class Vector3f
{
public:
    // constructors
    __EAPI Vector3f ();
    __EAPI Vector3f (float x, float y, float z);
    __EAPI Vector3f (const Vector4f& v4);	

    // initializer list constructor
    __EAPI Vector3f (std::initializer_list<float> i);

    // copy constructor
    __EAPI Vector3f (const Vector3f& other);

    // move constructor
    __EAPI Vector3f (Vector3f&& other);

    // assignment operator
    __EAPI Vector3f& operator= (Vector3f other);

    // destructor
    __EAPI ~Vector3f ();

    // vector dimension
    __EAPI unsigned int Dim () const;

    // accessor and mutator
    __EAPI float& operator[](unsigned int index);
    __EAPI const float& operator[](unsigned int index) const;

    // operators
    __EAPI Vector3f& operator*= (float f);
    __EAPI Vector3f& operator/= (float f);
    __EAPI Vector3f& operator+= (const Vector3f& other);
    __EAPI Vector3f& operator-= (const Vector3f& other);

    __EAPI bool operator== (const Vector3f& other) const;
    
    __EAPI friend Vector3f operator- (Vector3f v);

private:
    float v[3];
};

__EAPI Vector3f operator* (Vector3f u, float f);
__EAPI Vector3f operator* (float f, Vector3f u);
__EAPI Vector3f operator/ (Vector3f u, float f);
__EAPI Vector3f operator+ (Vector3f u, const Vector3f& v);
__EAPI Vector3f operator- (Vector3f u, const Vector3f& v);
__EAPI Vector3f operator- (Vector3f v);

} // namespace cilantro

#endif

