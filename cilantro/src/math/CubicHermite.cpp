#include "math/CubicHermite.h"
#include "math/Vector3f.h"
#include "math/Mathf.h"
#include <vector>

template <typename T> class CubicHermite;

template <typename T>
CubicHermite<T>::CubicHermite ()
{
}

template <typename T>
CubicHermite<T>::CubicHermite (const T& startPoint, const T& endPoint, const T& startTangent, const T& endTangent) : CubicHermite<T> ()
{
    SetPointsAndTangents (startPoint, endPoint, startTangent, endTangent);
}

template <typename T>
CubicHermite<T>::~CubicHermite ()
{
}

template <typename T>
CubicHermite<T>& CubicHermite<T>::SetPointsAndTangents (const T& startPoint, const T& endPoint, const T& startTangent, const T& endTangent)
{
    hMatrix = std::vector<std::vector<float>>{{0, 0, 0, 1}, {1, 1, 1, 1}, {0, 0, 1, 0}, {3, 2, 1, 0}};

    p0 = startPoint;
    p1 = endPoint;
    tp0 = startTangent;
    tp1 = endTangent;

    vCoefficients = std::vector<T>{p0, p1, tp0, tp1};
    Mathf::SolveSystemOfLinearEquations (hMatrix, vCoefficients);

    return *this;
}

template <typename T>
T CubicHermite<T>::GetCurvePoint (float t) const
{
    T point;
    float t2 = t * t;
    float t3 = t2 * t;

    point = vCoefficients[0] * t3 + vCoefficients[1] * t2 + vCoefficients[2] * t + vCoefficients[3];

    return point;
}

template <typename T>
T CubicHermite<T>::GetCurveTangent (float t) const
{
    T derivative;

    derivative = vCoefficients[0] * 3 * t * t + vCoefficients[1] * 2 * t + vCoefficients[2];

    return derivative;
}

// template instantiations
template class CubicHermite<Vector3f>;

