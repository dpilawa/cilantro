#include "math/Curve.h"
#include "math/Bezier.h"
#include "math/Mathf.h"
#include <vector>
#include <cmath>

namespace cilantro {

template <typename T, int d> class Bezier;

template <typename T, int d>
Bezier<T, d>::Bezier () : Curve<T, d> ()
{
}

template <typename T, int d>
Bezier<T, d>::Bezier (const std::vector<T>& p) : Bezier <T, d> ()
{
    points = p;
}

template <typename T, int d>
Bezier<T,d>::~Bezier ()
{
}

template <typename T, int d>
Bezier<T,d>& Bezier<T,d>::SetPoints (const std::vector<T>& p)
{
    points = p;

    return *this;
}

template <typename T, int d>
bool Bezier<T,d>::Validate () const
{
    return (points.size() == degree + 1);
}

template <typename T, int d>
T Bezier<T,d>::GetCurvePoint (float t) const
{
    T point;

    point = std::pow (1.0f - t, static_cast<float>(degree)) * points[0] + std::pow (t, static_cast<float>(degree)) * points[degree];
    for (unsigned int i = 1; i <= degree - 1; i++)
    {
        point += Mathf::Binomial (degree, i) * std::pow (1.0f - t, static_cast<float>(degree - i)) * std::pow (t, static_cast<float>(i)) * points[i];
    }

    return point;
}

template <typename T, int d>
T Bezier<T,d>::GetCurveTangent (float t) const
{
    T point;

    point = std::pow (1.0f - t, static_cast<float>(degree - 1)) * (points[1] - points[0]) + std::pow (t, static_cast<float>(degree - 1)) * (points[degree] - points[degree - 1]);
    for (unsigned int i = 1; i <= degree - 2; i++)
    {
        point += Mathf::Binomial (degree - 1, i) * std::pow (1.0f - t, static_cast<float>(degree - i)) * std::pow (t, static_cast<float>(i)) * (points[i + 1] - points[i]);
    }
    point *= static_cast<float>(degree);

    return point;
}

// template instantiations
template class Bezier<Vector3f, 2>;
template class Bezier<Vector3f, 3>;

} // namespace cilantro