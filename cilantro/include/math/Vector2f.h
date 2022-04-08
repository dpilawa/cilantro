#ifndef _VECTOR2F_H_
#define _VECTOR2F_H_

#include "cilantroengine.h"
#include <algorithm>
#include <initializer_list>

// Represents 2-dimensional float vector
class Vector2f
{
public:
    // constructors
    __EAPI Vector2f ();
    __EAPI Vector2f (float x, float y);

    // initializer list constructor
    __EAPI Vector2f (std::initializer_list<float> i);

    // copy constructor
    __EAPI Vector2f (const Vector2f& other);

    // move constructor
    __EAPI Vector2f (Vector2f&& other);

    // assignment operator
    __EAPI Vector2f& operator= (Vector2f other);

    // destructor
    __EAPI ~Vector2f ();

    // vector dimension
    __EAPI unsigned int Dim () const;

    // accessor and mutator
    __EAPI float& operator[](unsigned int index);
    __EAPI const float& operator[](unsigned int index) const;

    // operators
    __EAPI Vector2f& operator*= (float f);
    __EAPI Vector2f& operator/= (float f);
    __EAPI Vector2f& operator+= (const Vector2f& other);
    __EAPI Vector2f& operator-= (const Vector2f& other);
    
    __EAPI bool operator== (const Vector2f& other) const;

    __EAPI friend Vector2f operator- (Vector2f v);

private:
    float v[2];
};

__EAPI Vector2f operator* (Vector2f u, float f);
__EAPI Vector2f operator* (float f, Vector2f u);
__EAPI Vector2f operator/ (Vector2f u, float f);
__EAPI Vector2f operator+ (Vector2f u, const Vector2f& v);
__EAPI Vector2f operator- (Vector2f u, const Vector2f& v);
__EAPI Vector2f operator- (Vector2f v);

#endif

