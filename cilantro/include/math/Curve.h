#ifndef _CURVE_H_
#define _CURVE_H_

#include "cilantroengine.h"
#include "math/Vector3f.h"

namespace cilantro {

template <typename T, int d>
class __CEAPI Curve
{
public:
    __EAPI Curve ();
    __EAPI virtual ~Curve ();

    __EAPI virtual T GetCurvePoint (float t) const = 0;
    __EAPI virtual T GetCurveTangent (float t) const = 0;

    __EAPI float GetCurveLength ();

protected:
    unsigned int degree;
};

} // namespace cilantro

#endif