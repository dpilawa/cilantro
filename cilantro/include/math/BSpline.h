#ifndef _BSPLINE_H_
#define _BSPLINE_H_

#include "cilantroengine.h"
#include "math/Curve.h"
#include <vector>

namespace cilantro {

enum class KnotVectorType
{
    Clamped,
    Uniform
};

template <typename T, int d>
class BSpline : public Curve<T, d>
{
public:
    __EAPI BSpline ();
    __EAPI virtual ~BSpline ();

    __EAPI virtual T GetCurvePoint (float t) const;
    __EAPI virtual T GetCurveTangent (float t) const;

    __EAPI BSpline<T, d>& CalculateKnotVector (KnotVectorType type = KnotVectorType::Clamped);
    __EAPI BSpline<T, d>& SetKnotVector (const std::vector<float>& knotVector);

    __EAPI BSpline<T, d>& AddControlPoint (const T& point);
    __EAPI BSpline<T, d>& AddControlPoints (const std::vector<T>& points);
    __EAPI size_t GetControlPointsCount () const;
    __EAPI T GetControlPoint (size_t i) const;

    __EAPI virtual bool Validate ();

protected:
    using Curve<T, d>::degree;
    std::vector<T> controlPoints;
    std::vector<float> knots;
};

float Nip (size_t i, size_t p, const std::vector<float>& knots, float u);

template <typename T>
T EvaluateCurvePoint (unsigned int degree, const std::vector<T>& controlPoints, const std::vector<float>& knots, float u);

template <typename T>
T EvaluateCurveDerivative (unsigned int degree, const std::vector<T>& controlPoints, const std::vector<float>& knots, float u);

} // namespace cilantro

#endif