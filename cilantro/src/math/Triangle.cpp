#include "math/Triangle.h"
#include "math/Vector4f.h"
#include "math/Vector3f.h"
#include "math/Vector2f.h"

namespace cilantro {

// constructors
template <typename T>
Triangle<T>::Triangle ()
{ 

}

template <typename T>
Triangle<T>::Triangle (const T& p1, const T& p2, const T& p3) : p { p1, p2, p3 } 
{ 

}

// initializer list constructor
template <typename T>
Triangle<T>::Triangle (std::initializer_list<T> initializer)
{
    T* pPtr = p;

    for (auto&& i : initializer)
    {
        *pPtr++ = i;
    }
}

// copy constructor
template <typename T>
Triangle<T>::Triangle (const Triangle<T>& other)
{
    std::copy (other.p, other.p + 3, p);
}

// move constructor
template <typename T>
Triangle<T>::Triangle (Triangle<T>&& other) : Triangle<T> ()
{
    p[0] = std::move (other.p[0]);
    p[1] = std::move (other.p[1]);
    p[2] = std::move (other.p[2]);
}

// assignment operator
template <typename T>
Triangle<T>& Triangle<T>::operator= (Triangle<T> other)
{
    std::swap (p, other.p);
    return *this;
}

// destructor
template <typename T>
Triangle<T>::~Triangle ()
{ 

}

// mutator
template <typename T>
T& Triangle<T>::Triangle::operator[] (unsigned int index)
{
    return p[index];
}

// accessor
template <typename T>
const T& Triangle<T>::Triangle::operator[] (unsigned int index) const
{
    return p[index];
}


// explicit instantiation of template class
template class __CEAPI Triangle<Vector4f>;
template class __CEAPI Triangle<Vector3f>;
template class __CEAPI Triangle<Vector2f>;
template class __CEAPI Triangle<bool>;

} // namespace cilantro
    