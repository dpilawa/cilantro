#ifndef _SPLINE_H_
#define _SPLINE_H_

#include "cilantroengine.h"
#include "math/Vector3f.h"
#include <vector>

template <typename T>
class __EAPI Spline
{
public:
    Spline ();
    virtual ~Spline ();

    Spline<T>& SetDegree (unsigned int degree);
    unsigned int GetDegree () const;

    Spline<T>& AddControlPoint (const T& point);
    Spline<T>& AddControlPoints (const std::vector<T>& points);
    unsigned int GetControlPointsCount () const;
    T GetControlPoint(unsigned int i) const;

    virtual bool Validate () = 0;
    virtual T GetSplinePoint (float t) const = 0;

protected:
    unsigned int splineDegree;
    std::vector<T> controlPoints;
};

#endif