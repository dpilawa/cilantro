#ifndef _SPLINE_H_
#define _SPLINE_H_

#include "cilantroengine.h"
#include "math/Vector3f.h"
#include <vector>

template <typename T>
class Spline
{
public:
    __EAPI Spline ();
    __EAPI virtual ~Spline ();

    __EAPI Spline<T>& SetDegree (unsigned int degree);
    __EAPI unsigned int GetDegree () const;

    __EAPI Spline<T>& AddControlPoint (const T& point);
    __EAPI Spline<T>& AddControlPoints (const std::vector<T>& points);
    __EAPI unsigned int GetControlPointsCount () const;
    __EAPI T GetControlPoint(unsigned int i) const;

    virtual bool Validate () = 0;
    virtual T GetSplinePoint (float t) const = 0;

protected:
    unsigned int splineDegree;
    std::vector<T> controlPoints;
};

// template instantiations
template class __EAPI Spline<Vector3f>;

#endif