#include "math/NURBS.h"
#include "math/BSpline.h"
#include "math/Vector3f.h"
#include "math/Vector4f.h"
#include "math/Mathf.h"

namespace cilantro {

template <typename T, int d> class NURBS;

template <typename T, int d>
NURBS<T, d>::NURBS () : BSpline<T, d> ()
{
}

template <typename T, int d>
NURBS<T, d>::~NURBS ()
{
}

template <typename T, int d>
T NURBS<T, d>::GetCurvePoint (float t) const
{
    T point = EvaluateCurvePoint (degree, controlPoints, knots, weights, t);

    return point;
}

template <typename T, int d>
T NURBS<T, d>::GetCurveTangent (float t) const
{
    T derivative = EvaluateCurveDerivative (degree, controlPoints, knots, weights, t);

    return derivative;
}

template <typename T, int d>
NURBS<T, d>& NURBS<T, d>::SetWeights (const std::vector<float>& weightVector)
{
    weights.clear ();

    for (unsigned int i = 0; i < weightVector.size (); i++)
    {
        weights.push_back (weightVector[i]);
    }

    return *this;
}

template <typename T, int d>
bool NURBS<T, d>::Validate ()
{
    size_t m = knots.size ();
    size_t n = controlPoints.size ();
    size_t w = weights.size ();
    unsigned int p = degree;

    if ((n == m - p - 1) && (n == w))
    {
        return true;
    }
    else
    {
        return false;
    }
}

template <typename T>
T EvaluateCurvePoint (unsigned int degree, const std::vector<T>& controlPoints, const std::vector<float>& knots, const std::vector<float>& weights, float u)
{
    T point;

    float rationalWeight = 0.0f;

    for (size_t i = 0; i < controlPoints.size (); i++)
    {
        rationalWeight += Nip (i, degree, knots, u) * weights[i];
    }

    for (size_t i = 0; i < controlPoints.size (); i++)
    {
        point = point + controlPoints[i] * (Nip (i, degree, knots, u) * weights[i] / rationalWeight);
    }

    return point;
}

template <typename T>
T EvaluateCurveDerivative (unsigned int degree, const std::vector<T>& controlPoints, const std::vector<float>& knots, const std::vector<float>& weights, float u)
{
    T derivative;

    auto cw = Mathf::CartesianToHomogenous (controlPoints, weights);
    auto cwd = EvaluateCurveDerivative (degree, cw, knots, u);
    auto point = EvaluateCurvePoint (degree, cw, knots, u);

    derivative = (Mathf::HomogenousToCartesianTruncate (cwd) - Mathf::GetHomogenousWeight (cwd) * Mathf::HomogenousToCartesianPerspective (point)) * (1.0f / point[point.Dim () - 1]);

    return derivative;
}

// template instantiations
template class NURBS<Vector3f, 2>;
template class NURBS<Vector3f, 3>;

} // namespace cilantro

