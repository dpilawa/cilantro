#ifndef _BEZIER_H_
#define _BEZIER_H_

#include "cilantroengine.h"
#include "math/Curve.h"
#include <vector>

template <typename T, int d>
class Bezier : public Curve<T, d>
{
public:
    __EAPI Bezier ();
    __EAPI Bezier (const std::vector<T>& p);
    __EAPI virtual ~Bezier ();

    __EAPI Bezier<T, d>& SetPoints (const std::vector<T>& p);
    __EAPI bool Validate () const;

    __EAPI T GetCurvePoint (float t) const;
    __EAPI T GetCurveTangent (float t) const;

private:
    using Curve<T, d>::degree;

    std::vector<T> points;
};

#endif