#ifndef _GAUSSLEGENDREINTEGRATOR_H_
#define _GAUSSLEGENDREINTEGRATOR_H_

#define INTEGRATOR_DEGREE 100

#include "cilantroengine.h"
#include <tuple>
#include <vector>
#include <functional>

template<int n>
class GaussLegendreIntegrator
{
public:
    __EAPI GaussLegendreIntegrator ();
    __EAPI ~GaussLegendreIntegrator ();

    __EAPI float Integral (float a, float b, std::function<float(float)> f);

private:
    std::vector<float> nodes;
    std::vector<float> weights;

    std::tuple<float, float> PolynomialEvaluate (float x);
};

#endif