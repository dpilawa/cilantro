#ifndef _MATRIX4F_H_
#define _MATRIX4F_H_

#include "cilantroengine.h"
#include "Vector4f.h"

namespace cilantro {

class Matrix4f
{
public:
    // constructor
    __EAPI Matrix4f ();
    
    // initializer list constructor
    __EAPI Matrix4f (std::initializer_list<float> i);

    // copy constructor
    __EAPI Matrix4f (const Matrix4f& other);

    // copy constructor (column vectors)
    __EAPI Matrix4f (const Vector4f& c1, const Vector4f& c2, const Vector4f& c3, const Vector4f& c4);

    // move constructor
    __EAPI Matrix4f (Matrix4f&& other);

    // assignment operator
    __EAPI Matrix4f& operator= (Matrix4f other);

    // destructor
    __EAPI ~Matrix4f ();

    // accessor and mutator
    __EAPI float* operator[] (unsigned int index);
    __EAPI const float* operator[] (unsigned int index) const;

    // methods
    __EAPI Matrix4f& InitIdentity ();

    // operators
    __EAPI Matrix4f& operator*= (const Matrix4f& m);
    __EAPI Vector4f operator*(const Vector4f& v) const;
    __EAPI Matrix4f& operator*= (float f);

private:
    float m[16];

};

// binary operator for matrix by matrix multiplication
__EAPI Matrix4f operator* (Matrix4f m, const Matrix4f& n);

// binary operator for matrix by float multiplication
__EAPI Matrix4f operator* (Matrix4f m, float f);
__EAPI Matrix4f operator* (float f, Matrix4f m);

} // namespace cilantro

#endif