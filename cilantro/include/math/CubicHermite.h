#ifndef _CUBICHERMITE_H_
#define _CUBICHERMITE_H_

#include "cilantroengine.h"
#include "math/Curve.h"
#include <vector>

namespace cilantro {

template <typename T>
class CubicHermite : public Curve<T, 3>
{
public:
    __EAPI CubicHermite ();
    __EAPI CubicHermite (const T& startPoint, const T& endPoint, const T& startTangent, const T& endTangent);
    __EAPI virtual ~CubicHermite ();

    __EAPI CubicHermite<T>& SetPointsAndTangents (const T& startPoint, const T& endPoint, const T& startTangent, const T& endTangent);

    __EAPI T GetCurvePoint (float t) const;
    __EAPI T GetCurveTangent (float t) const;

private:
    T p0, p1;
    T tp0, tp1;

    std::vector<std::vector<float>> hMatrix;
    std::vector<T> vCoefficients;
};

} // namespace cilantro

#endif