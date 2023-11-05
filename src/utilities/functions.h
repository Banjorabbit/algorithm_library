#pragma once
#include "framework/framework.h"

// Approximation to modified Bessel function of first kind 0th order:
// https://elec424.fandom.com/wiki/Modified_Bessel_Functions#Approximations_for_Modified_Bessel_Functions_of_the_First_Kind
// 
// The input/output is an Eigen array and the approximation is hard coded to a certain order.
//
// author: Kristian Timm Andersen
inline Eigen::ArrayXf modifiedBesselFirstKindZeroOrder(I::Real x)
{
    constexpr int order = 6; // hardcoded to use 1 + order terms in approximations
    const auto N = x.size();
    float fac = 1; // factorial increasing in for-loop
    Eigen::ArrayXf i0 = Eigen::ArrayXf::Ones(N); // i0 approx that is accurate for small x
    for (auto i = 1; i <= order; i++)
    {
        fac *= i;
        i0 += x.pow(2 * i) / (std::pow(2, 2 * i) * fac * fac);
    }

    Eigen::ArrayXf i0a = Eigen::ArrayXf::Ones(N); // asymptotic approx that is accurate for large x
    float facNum = 1;
    float facDen = 1;
    Eigen::ArrayXf powDen = Eigen::ArrayXf::Ones(x.size());
    for (auto i = 1; i <= order; i++)
    {
        facNum *= (i*2-1)*(i*2-1);
        facDen *= i;
        powDen *= 8 * x;
        i0a += facNum / (facDen * powDen);
    }
    i0a *= x.exp() / (2.506628274631f * x.sqrt()); // exp(x) / sqrt(2 * pi * x) = exp(x) / ( sqrt(2 * pi) * sqrt(x) )

    return (x > 5.f).select(i0a, i0); // use asymptotic approximation for x larger than 5
}

// Approximation to modified Bessel function of first kind 0th order:
// https://elec424.fandom.com/wiki/Modified_Bessel_Functions#Approximations_for_Modified_Bessel_Functions_of_the_First_Kind
// 
// The input/output is a float scalar and the approximation is hard coded to a certain order.
//
// author: Kristian Timm Andersen
inline float modifiedBesselFirstKindZeroOrder(const float x)
{
    constexpr int order = 6; // hardcoded to use 1 + order terms in approximations
    float i0 = 1;
    if (x > 5) // use asymptotic approximation
    {
        float facNum = 1;
        float facDen = 1; // factorial increasing in for-loop
        float powDen = 1;
        for (auto i = 1; i <= order; i++)
        {
            facNum *= (i * 2 - 1) * (i * 2 - 1);
            facDen *= i;
            powDen *= 8 * x;
            i0 += facNum / (facDen * powDen);
        }
        i0 *= std::exp(x) / (2.506628274631f * std::sqrt(x)); // exp(x) / sqrt(2 * pi * x) = exp(x) / ( sqrt(2 * pi) * sqrt(x) )
    }
    else
    {
        float fac = 1; // factorial increasing in for-loop
        float powNum = 1;
        float Den4 = 1;
        for (auto i = 1; i <= order; i++)
        {
            fac *= i;
            powNum *= x * x;
            Den4 *= 4;
            i0 += powNum / (Den4 * fac * fac);
        }
    }
    return i0;
}

// kaiser returns a beta-valued nSamples-point Kaiser window
//
// author: Kristian Timm Andersen
inline Eigen::ArrayXf kaiser(const int nSamples, const float beta)
{
    const float bes = modifiedBesselFirstKindZeroOrder(beta);
    const int odd = nSamples % 2;
    const int xInd = (nSamples - 1) * (nSamples - 1);
    const int n = (nSamples + 1) / 2;
    Eigen::ArrayXf xi = Eigen::ArrayXf::LinSpaced(n, 0, static_cast<float>(n - 1)) + .5f * (1 - odd);
    xi *= 4 * xi;
    xi = beta * (1 - xi / xInd).sqrt();
    const Eigen::ArrayXf window = modifiedBesselFirstKindZeroOrder(xi) / bes;
    Eigen::ArrayXf windowCon(window.size() + n - odd);
    windowCon << window.segment(odd, n - odd).reverse(), window;
    return windowCon;
}

// hann window
//
// author: Kristian Timm Andersen
inline Eigen::ArrayXf hann(const int nSamples)
{
    return .5f * (1 - Eigen::ArrayXd::LinSpaced(nSamples, 0, 6.283185307179586 * (nSamples - 1) / nSamples).cos()).cast<float>();
}

// Sinc window. If nSamples is odd, the window is symmetric. If nSamples is even, the window is periodic.
// zcr is number of zero-crossings on each side of the middlepoint.
//
// author: Kristian Timm Andersen
inline Eigen::ArrayXf sinc(const int nSamples, const int zcr)
{
    if (nSamples % 2 == 0)
    {
        const int n2 = nSamples / 2;
        Eigen::ArrayXd input = Eigen::ArrayXd::LinSpaced(n2, -zcr * 3.141592653589793, -2 * zcr * 3.141592653589793 / nSamples);
        input = input.sin() / input;
        Eigen::ArrayXf output(nSamples);
        output << input.cast<float>(), 1, input.tail(n2 - 1).reverse().cast<float>();
        return output;
    }
    else
    {
        Eigen::ArrayXd input = Eigen::ArrayXd::LinSpaced((nSamples-1)/2, (-zcr + zcr/nSamples) * 3.141592653589793, 2 * zcr / nSamples * 3.141592653589793);
        input = input.sin() / input;
        Eigen::ArrayXf output(nSamples);
        output << input.cast<float>(), 1, input.reverse().cast<float>();
        return output;
    }
}

static inline float
fastPow2 (float p)
{
  float offset = (p < 0) ? 1.0f : 0.0f;
  float clipp = (p < -126) ? -126.0f : p;
  int w = clipp;
  float z = clipp - w + offset;
  union { uint32_t i; float f; } v = { static_cast<uint32_t>( (1 << 23) * (clipp + 121.2740575f + 27.7280233f / (4.84252568f - z) - 1.49012907f * z) ) };

  return v.f;
}

static inline float
fastExp (float p)
{
  return fastPow2 (1.442695040f * p);
}

static inline float
fasterPow2 (float p)
{
  float clipp = (p < -126) ? -126.0f : p;
  union { uint32_t i; float f; } v = { static_cast<uint32_t>( (1 << 23) * (clipp + 126.94269504f) ) };
  return v.f;
}

static inline float
fasterExp (float p)
{
  return fasterPow2 (1.442695040f * p);
}