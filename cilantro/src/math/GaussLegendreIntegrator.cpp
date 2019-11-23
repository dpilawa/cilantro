#include "math/GaussLegendreIntegrator.h"
#include "math/Mathf.h"
#include <tuple>
#include <cmath>
#include <limits>

template <int n> class GaussLegendreIntegrator;

template <int n>
GaussLegendreIntegrator<n>::GaussLegendreIntegrator ()
{
    // pre-calculate nodes and weights
    for (int i = 1; i <= n; i++)
    {
        // Newton-Rhapson iteration
        float x = std::cos (Mathf::Pi () * (i - 0.25f) / (n + 0.5f));
        float x1;
        float f, d;

        do 
        {
            x1 = x;
            std::tie (f, d) = PolynomialEvaluate (x);
            x = x - f / d;
        } while (std::abs (x1 - x) > std::numeric_limits<float>::epsilon ());
        
        std::tie (f, d) = PolynomialEvaluate (x);
        nodes.push_back (x);
        weights.push_back (2.0f / ((1.0f - x * x) * d * d));

    }

}

template <int n>
GaussLegendreIntegrator<n>::~GaussLegendreIntegrator ()
{
}

template <int n>
float GaussLegendreIntegrator<n>::Integral (float a, float b, std::function<float(float)> f)
{
    float integral = 0.0f;

    for (int i = 0; i < n; i++)
    {
        integral += weights[i] * f ((b - a) * 0.5f * nodes[i] + (a + b) * 0.5f);
    }

    return integral * (b - a) * 0.5f;
}

template <int n>
std::tuple<float, float> GaussLegendreIntegrator<n>::PolynomialEvaluate (float x)
{
    float f, d;
    float v0 = 1.0f;
    float v1 = x;

    for (int i = 2; i <= n; i++)
    {
        f = ((2 * i - 1) * v1 * x - (i - 1) * v0) / i;
        d = (i / (x * x - 1)) * (x * f - v1);
        v0 = v1;
        v1 = f;
    }

    return std::make_tuple (f, d);
}

// template instantiations
template class GaussLegendreIntegrator<INTEGRATOR_DEGREE>;