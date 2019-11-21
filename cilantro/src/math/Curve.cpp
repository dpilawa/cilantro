#include "math/Curve.h"
#include "math/Vector3f.h"
#include "math/Vector4f.h"

template <typename T, int d> class Curve;

template <typename T, int d>
Curve<T,d>::Curve ()
{
    degree = d;
}

template <typename T, int d>
Curve<T,d>::~Curve ()
{
}

// template instantiations
template class Curve<Vector3f, 2>;
template class Curve<Vector3f, 3>;

