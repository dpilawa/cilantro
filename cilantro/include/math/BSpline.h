#ifndef _BSpline_H_
#define _BSpline_H_

#include "cilantroengine.h"
#include "math/Spline.h"
#include "math/Vector3f.h"

enum class KnotVectorType
{
	Clamped,
	Uniform
};

template <typename T>
class BSpline : public Spline<T>
{
public:
    __EAPI BSpline ();
    __EAPI virtual ~BSpline ();

    __EAPI virtual T GetSplinePoint (float t) const;

    __EAPI BSpline<T>& CalculateKnotVector (KnotVectorType type = KnotVectorType::Clamped);
    __EAPI BSpline<T>& SetKnotVector (const std::vector<float>& knotVector);

    __EAPI virtual bool Validate ();

protected:
    using Spline<T>::splineDegree;
    using Spline<T>::controlPoints;

    float Nip (int i, int p, const std::vector<float>& knots, float u) const;

    std::vector<float> knots;
};

#endif