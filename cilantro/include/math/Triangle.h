#ifndef _TRIANGLE_H_
#define _TRIANGLE_H_

#include "cilantroengine.h"
#include <algorithm>
#include <initializer_list>

namespace cilantro {

// Represents a triangle
template <typename T>
class __CEAPI Triangle
{
public:
    // constructors
    __EAPI Triangle ();
    __EAPI Triangle (const T& p1, const T& p2, const T& p3);
    
    // initializer list constructor
    __EAPI Triangle (std::initializer_list<T> i);

    // copy constructor
    __EAPI Triangle (const Triangle<T>& other);

    // move constructor
    __EAPI Triangle (Triangle<T>&& other);

    // assignment operator
    __EAPI Triangle<T>& operator= (Triangle<T> other);

    // destructor
    __EAPI ~Triangle ();

    // accessor and mutator
    __EAPI T& operator[](unsigned int index);
    __EAPI const T& operator[](unsigned int index) const;

private:
    T p[3];

};


} // namespace cilantro

#endif

