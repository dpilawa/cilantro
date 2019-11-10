#include "math/BSpline.h"
#include "math/Vector3f.h"

template <typename T> class BSpline;

template <typename T>
BSpline<T>::BSpline ()
{
 
}

template <typename T>
BSpline<T>::~BSpline ()
{

}

template <typename T>
T BSpline<T>::GetSplinePoint (float t) const
{
    T point;

    for (int i = 0; i < controlPoints.size (); i++)
    {
        point = point + controlPoints[i] * Nip (i, splineDegree, knots, t);
    }

    return point;
}

template <typename T>
BSpline<T>& BSpline<T>::CalculateKnotVector (KnotVectorType type)
{
    unsigned int n = this->controlPoints.size ();
    unsigned int p = this->splineDegree;

    knots.clear ();
    knots.resize (n + p + 1);

    if (type == KnotVectorType::Clamped)
    {
        for (int m = 0; m < p + 1; m++)
        {
            knots[m] = 0.0f;
        }

        for (int m = p + 1; m < n; m++)
        {
            knots[m] = static_cast<float>(m - p) / static_cast<float>(n - p);
        }

        for (int m = n; m < n + p + 1; m++)
        {
            knots[m] = 1.0f;
        }
    }
    else
    {
        for (int m = 0; m < n + p + 1; m++)
        {
            knots[m] = static_cast<float>(m + 1) / static_cast<float>(n + p + 1);
        }
    }

    return *this;
}

template <typename T>
BSpline<T>& BSpline<T>::SetKnotVector (const std::vector<float>& knotVector)
{
    knots.clear ();

    for (int i = 0; i < knotVector.size (); i++)
    {
        knots.push_back (knotVector[i]);
    }

    return *this;
}

template <typename T>
bool BSpline<T>::Validate ()
{
    unsigned int m = knots.size ();
    unsigned int n = controlPoints.size ();
    unsigned int p = splineDegree;

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
template <typename T>
float BSpline<T>::Nip (int i, int p, const std::vector<float>& U, float u) const
{
    std::vector<float> N;
    N.resize (p + 1);

    int m = U.size () - 1;

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

// template instantiations
template class BSpline<Vector3f>;

