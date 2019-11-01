#ifndef _SPLINE_H_
#define _SPLINE_H_

#include <vector>

template <typename T>
class Spline
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

template <typename T>
Spline<T>::Spline ()
{
    splineDegree = 1;
}

template <typename T>
Spline<T>::~Spline ()
{
}

template <typename T>
Spline<T>& Spline<T>::SetDegree (unsigned int degree)
{
    splineDegree = degree;

    return *this;
}

template <typename T>
unsigned int Spline<T>::GetDegree () const
{
    return splineDegree;
}

template <typename T>
Spline<T>& Spline<T>::AddControlPoint (const T& point)
{
    controlPoints.push_back (point);

    return *this;
}

template <typename T>
Spline<T>& Spline<T>::AddControlPoints (const std::vector<T>& points)
{
    controlPoints = points;

    return *this;
}

template <typename T>
unsigned int Spline<T>::GetControlPointsCount () const
{
    return controlPoints.size ();
}

template <typename T>
T Spline<T>::GetControlPoint (unsigned int i) const
{
    return controlPoints[i];
}

#endif