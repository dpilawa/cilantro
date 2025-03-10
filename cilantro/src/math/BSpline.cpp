#include "math/BSpline.h"
#include "math/Vector3f.h"
#include "math/Vector4f.h"
#include "math/Mathf.h"

namespace cilantro {

template <typename T, int d> class BSpline;

template <typename T, int d>
BSpline<T, d>::BSpline ()
{
}

template <typename T, int d>
BSpline<T, d>::~BSpline ()
{
}

template <typename T, int d>
T BSpline<T, d>::GetCurvePoint (float t) const
{
    T point = EvaluateCurvePoint (degree, controlPoints, knots, t);

    return point;
}

template <typename T, int d>
T BSpline<T, d>::GetCurveTangent (float t) const
{
    T derivative = EvaluateCurveDerivative (degree, controlPoints, knots, t);

    return derivative;
}

template <typename T, int d>
BSpline<T, d>& BSpline<T, d>::CalculateKnotVector (KnotVectorType type)
{
    size_t n = controlPoints.size ();
    unsigned int p = degree;

    knots.clear ();
    knots.resize (n + p + 1);

    if (type == KnotVectorType::Clamped)
    {
        for (size_t m = 0; m < p + 1; m++)
        {
            knots[m] = 0.0f;
        }

        for (size_t m = p + 1; m < n; m++)
        {
            knots[m] = static_cast<float>(m - p) / static_cast<float>(n - p);
        }

        for (size_t m = n; m < n + p + 1; m++)
        {
            knots[m] = 1.0f;
        }
    }
    else
    {
        for (size_t m = 0; m < n + p + 1; m++)
        {
            knots[m] = static_cast<float>(m + 1) / static_cast<float>(n + p + 1);
        }
    }

    return *this;
}

template <typename T, int d>
BSpline<T, d>& BSpline<T, d>::SetKnotVector (const std::vector<float>& knotVector)
{
    knots.clear ();

    for (size_t i = 0; i < knotVector.size (); i++)
    {
        knots.push_back (knotVector[i]);
    }

    return *this;
}

template <typename T, int d>
BSpline<T, d>& BSpline<T, d>::AddControlPoint (const T& point)
{
    controlPoints.push_back (point);

    return *this;
}

template <typename T, int d>
BSpline<T, d>& BSpline<T, d>::AddControlPoints (const std::vector<T>& points)
{
    controlPoints = points;

    return *this;
}

template <typename T, int d>
size_t BSpline<T, d>::GetControlPointsCount () const
{
    return controlPoints.size ();
}

template <typename T, int d>
T BSpline<T, d>::GetControlPoint (size_t i) const
{
    return controlPoints[i];
}

template <typename T, int d>
bool BSpline<T, d>::Validate ()
{
    size_t m = knots.size ();
    size_t n = controlPoints.size ();
    unsigned int p = degree;

    if (n == m - p - 1)
    {
        return true;
    }
    else
    {
        return false;
    }
}

// the original de Boor algorithm (source: "The BURBS Book")
float Nip (size_t i, size_t p, const std::vector<float>& U, float u)
{
    std::vector<float> N;
    N.resize (p + 1);

    size_t m = U.size () - 1;

    // handle special case (convention)
    if ((i == 0 && u == U[0]) || (i == (m - p - 1) && u == U[m]))
    {
        return 1.0f;
    }

    // zero outside of span
    if (u < U[i] || u >= U[i + p + 1])
    {
        return 0.0f;
    }

    for (int j = 0; j <= p; j++)
    {
        N[j] = (u >= U[i + j] && u < U[i + j + 1]) ? 1.0f : 0.0f;
    }

    for (int k = 1; k <= p; k++)
    {
        float saved = (N[0] == 0.0f) ? 0.0f : ((u - U[i]) * N[0]) / (U[i + k] - U[i]);

        for (int j = 0; j < p - k + 1; j++)
        {
            float Uleft = U[i + j + 1];
            float Uright = U[i + j + k + 1];

            if (N[j + 1] == 0)
            {
                N[j] = saved;
                saved = 0.0f;
            }
            else
            {
                float temp = N[j + 1] / (Uright - Uleft);
                N[j] = saved + (Uright - u) * temp;
                saved = (u - Uleft) * temp;
            }
        }
    }

    return N[0];
}

template <typename T>
T EvaluateCurvePoint (unsigned int degree, const std::vector<T>& controlPoints, const std::vector<float>& knots, float u)
{
    T point;

    for (unsigned int i = 0; i < controlPoints.size (); i++)
    {
        point = point + controlPoints[i] * Nip (i, degree, knots, u);
    }

    return point;
}

template <typename T>
T EvaluateCurveDerivative (unsigned int degree, const std::vector<T>& controlPoints, const std::vector<float>& knots, float u)
{
    T derivative;

    for (unsigned int i = 0; i < controlPoints.size () - 1; i++)
    {
        derivative = derivative + (controlPoints[i + 1] - controlPoints[i]) * (degree / (knots[i + degree + 1] - knots[i + 1])) * Nip (i + 1, degree - 1, knots, u);
    }

    return derivative;
}

// template instantiations
template class BSpline<Vector3f, 2>;
template class BSpline<Vector3f, 3>;

template Vector3f EvaluateCurvePoint<Vector3f> (unsigned int degree, const std::vector<Vector3f>& controlPoints, const std::vector<float>& knots, float u);
template Vector3f EvaluateCurveDerivative<Vector3f> (unsigned int degree, const std::vector<Vector3f>& controlPoints, const std::vector<float>& knots, float u);
template Vector4f EvaluateCurvePoint<Vector4f> (unsigned int degree, const std::vector<Vector4f>& controlPoints, const std::vector<float>& knots, float u);
template Vector4f EvaluateCurveDerivative<Vector4f> (unsigned int degree, const std::vector<Vector4f>& controlPoints, const std::vector<float>& knots, float u);

} // namespace cilantro


