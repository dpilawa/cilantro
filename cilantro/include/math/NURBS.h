#ifndef _NURBS_H_
#define _NURBS_H_

#include "cilantroengine.h"
#include "math/BSpline.h"

template <typename T>
class NURBS : public BSpline<T>
{
public:
    NURBS (){};
    virtual ~NURBS (){};

    T GetSplinePoint (float t) const;

    NURBS<T>& SetWeights (const std::vector<float>& weightVector);

    bool Validate ();

protected:
    using Spline<T>::splineDegree;
    using Spline<T>::controlPoints;
    using BSpline<T>::knots;
    using BSpline<T>::Nip;

    std::vector<float> weights;
};

template <typename T>
T NURBS<T>::GetSplinePoint (float t) const
{
    T point;
    float rationalWeight = 0.0f;

    for (int i = 0; i < controlPoints.size (); i++)
    {
        rationalWeight += Nip (i, splineDegree, knots, t) * weights[i];
    }

    for (int i = 0; i < controlPoints.size (); i++)
    {
        point = point + controlPoints[i] * (Nip (i, splineDegree, knots, t) * weights[i] / rationalWeight);
    }

    return point;
}

template <typename T>
NURBS<T>& NURBS<T>::SetWeights (const std::vector<float>& weightVector)
{
    weights.clear ();

    for (int i = 0; i < weightVector.size (); i++)
    {
        weights.push_back (weightVector[i]);
    }

    return *this;
}

template <typename T>
bool NURBS<T>::Validate ()
{
    unsigned int m = knots.size ();
    unsigned int n = controlPoints.size ();
    unsigned int w = weights.size ();
    unsigned int p = splineDegree;

    if ((n == m - p - 1) && (n == w))
    {
        return true;
    }
    else
    {
        return false;
    }
}

#endif