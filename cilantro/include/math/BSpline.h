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
    BSpline ();
    virtual ~BSpline ();

    virtual T GetSplinePoint (float t) const;

    BSpline<T>& CalculateKnotVector (KnotVectorType type = KnotVectorType::Clamped);
    BSpline<T>& SetKnotVector (const std::vector<float>& knotVector);

    virtual bool Validate ();

protected:
    using Spline<T>::splineDegree;
    using Spline<T>::controlPoints;

    float Nip (int i, int p, const std::vector<float>& knots, float u) const;

    std::vector<float> knots;
};

#endif