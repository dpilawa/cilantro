#ifndef _NURBS_H_
#define _NURBS_H_

#include "cilantroengine.h"
#include "math/BSpline.h"
#include "math/Vector3f.h"

template <typename T>
class NURBS : public BSpline<T>
{
public:
    NURBS ();
    virtual ~NURBS ();

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

#endif