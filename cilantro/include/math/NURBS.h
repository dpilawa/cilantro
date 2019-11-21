#ifndef _NURBS_H_
#define _NURBS_H_

#include "cilantroengine.h"
#include "math/BSpline.h"
#include <vector>

template <typename T, int d>
class NURBS : public BSpline<T, d>
{
public:
    __EAPI NURBS ();
    __EAPI virtual ~NURBS ();

    __EAPI T GetCurvePoint (float t) const;
    __EAPI T GetCurveTangent (float t) const;

    __EAPI NURBS<T, d>& SetWeights (const std::vector<float>& weightVector);

    __EAPI bool Validate ();

protected:
    using Curve<T, d>::degree;
    using BSpline<T, d>::controlPoints;
    using BSpline<T, d>::knots;

    std::vector<float> weights;
};

template <typename T>
T EvaluateCurvePoint (unsigned int degree, const std::vector<T>& controlPoints, const std::vector<float>& knots, const std::vector<float>& weights, float u);

template <typename T>
T EvaluateCurveDerivative (unsigned int degree, const std::vector<T>& controlPoints, const std::vector<float>& knots, const std::vector<float>& weights, float u);

#endif