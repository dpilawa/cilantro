#ifndef _MATRIX3F_H_
#define _MATRIX3F_H_

#include "cilantroengine.h"
#include "math/Matrix4f.h"

namespace cilantro {

class __CEAPI Matrix3f
{
public:
    // constructor
    __EAPI Matrix3f ();

    // initializer list constructor
    __EAPI Matrix3f (std::initializer_list<float> i);

    // copy constructor
    __EAPI Matrix3f (const Matrix3f& other);

    // copy constructor (submatrix)
    __EAPI Matrix3f (const Matrix4f& other);

    // copy constructor (column vectors)
    __EAPI Matrix3f (const Vector3f& c1, const Vector3f& c2, const Vector3f& c3);

    // move constructor
    __EAPI Matrix3f (Matrix3f&& other);

    // assignment operator
    __EAPI Matrix3f& operator= (Matrix3f other);

    // destructor
    __EAPI ~Matrix3f ();

    // accessor and mutator
    __EAPI float* operator[](unsigned int index);
    __EAPI const float* operator[](unsigned int index) const;

    // methods
    __EAPI Matrix3f& InitIdentity ();

    // operators
    __EAPI Matrix3f& operator*= (const Matrix3f& m);
    __EAPI Vector3f operator*(const Vector3f& v) const;
    __EAPI Matrix3f& operator*= (float f);

private:
    float m[9];

};

// binary operator for matrix by matrix multiplication
__EAPI Matrix3f operator* (Matrix3f m, const Matrix3f& n);

// binary operator for matrix by float multiplication
__EAPI Matrix3f operator* (Matrix3f m, float f);
__EAPI Matrix3f operator* (float f, Matrix3f m);

} // namespace cilantro

#endif