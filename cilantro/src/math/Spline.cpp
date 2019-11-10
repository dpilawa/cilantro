#include "math/Spline.h"
#include "math/Vector3f.h"

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
