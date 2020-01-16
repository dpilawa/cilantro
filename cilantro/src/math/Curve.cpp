#include "math/Curve.h"
#include "math/Vector3f.h"
#include "math/Vector4f.h"
#include "math/Mathf.h"

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

template <typename T, int d>
float Curve<T, d>::GetCurveLength ()
{
    return Mathf::Integral (0.0f, 1.0f, [&](float t) { return Mathf::Length (GetCurveTangent (t)); });
}


// template instantiations
template class Curve<Vector3f, 2>;
template class Curve<Vector3f, 3>;

