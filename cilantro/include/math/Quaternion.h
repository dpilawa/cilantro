#ifndef _QUATERNION_H_
#define _QUATERNION_H_

#include "cilantroengine.h"
#include "math/Vector3f.h"

namespace cilantro {

class __CEAPI Quaternion
{
public:
    // constructors
    __EAPI Quaternion ();
    __EAPI Quaternion (float a, float b, float c, float d);
    __EAPI Quaternion (float scalar, Vector3f vector);

    // copy constructor
    __EAPI Quaternion (const Quaternion& other);

    // move constructor
    __EAPI Quaternion (Quaternion&& other);

    // assignment operator
    __EAPI Quaternion& operator= (Quaternion other);

    // destructor
    __EAPI ~Quaternion ();

    // operators
    __EAPI Quaternion& operator*= (float f);
    __EAPI Quaternion& operator+= (const Quaternion& other);
    __EAPI Quaternion& operator-= (const Quaternion& other);
    
    __EAPI friend Quaternion operator- (Quaternion v);

    friend class Mathf;

private:
    float s;
    Vector3f v;
};

__EAPI Quaternion operator* (Quaternion q, float f);
__EAPI Quaternion operator* (float f, Quaternion q);
__EAPI Quaternion operator+ (Quaternion q, const Quaternion& r);
__EAPI Quaternion operator- (Quaternion q, const Quaternion& r);
__EAPI Quaternion operator- (Quaternion q);

} // namespace cilantro

#endif

