#pragma once
#include "macros_json.h"
#include <Eigen/Dense> // Eigen Library.

// Define ConstrainedType, which defines a min/max limit for the templated type.
//
// Author: Kristian Timm Andersen

template <typename T>
class ConstrainedType
{
  public:
    // % Matlab plot of different Scalings:
    // vmin = 1;
    // vmax = 10;
    // v = linspace(vmin, vmax, 1000)';
    //	y = v;
    // ylog = log(v);
    // a = (vmin - vmax) / (ylog(1) - ylog(end));
    // b = vmax - ylog(end)*a;
    // ylog = ylog * a + b;
    // yexp = exp(v / 3);
    // a = (vmin - vmax) / (yexp(1) - yexp(end));
    // b = vmax - yexp(end)*a;
    // yexp = yexp * a + b;
    // plot(v, [y, ylog, yexp]);
    // legend('linear', 'log', 'exp', 'location', 'nw');
    // grid on;

    enum Scalings { LINEAR, LOGARITHMIC, EXPONENTIAL };
    enum Units { NA, SECONDS, MILLISECONDS, MICROSECONDS, HZ, KHZ, DB };

    ConstrainedType(const T &valueNew, const T &vMinNew, const T &vMaxNew) : vMin(vMinNew), vMax(vMaxNew)
    {
        *this = valueNew;
    }; // constructor, uses assignment operator to check value
    ConstrainedType(const T &valueNew, const T &vMinNew, const T &vMaxNew, const Scalings &scalingNew, const float &scaleFactorNew, const Units &unitNew)
        : vMin(vMinNew), vMax(vMaxNew), scaling(scalingNew), scaleFactor(scaleFactorNew), unit(unitNew)
    {
        *this = valueNew;
    }; // constructor, uses assignment operator to check value

    ConstrainedType &operator=(const T &valueNew) // assignment operator
    {
        // // this commented-out code can be used to set a break point if necessary
        // if (valueNew != (std::min)(vMax, valueNew))
        //{
        //	int dummy = 1;
        //}
        // if (valueNew != (std::max)(vMin, valueNew))
        //{
        //	int dummy = 1;
        //}

        // assert(valueNew == (std::min)(vMax, valueNew)); // catch in debug
        // assert(valueNew == (std::max)(vMin, valueNew)); // catch in debug
        value = (std::max)((std::min)(valueNew, vMax), vMin);
        return *this;
    }
    operator T() const
    {
        return value;
    } // cast to T type (used when assigning to other variable / expression). Some optimization error results in this failing if return type is changed to const reference

    void setMax(const T &vMaxNew)
    {
        vMax = vMaxNew;
        value = (std::min)(vMax, value);
    }
    void setMin(const T &vMinNew)
    {
        vMin = vMinNew;
        value = (std::max)(vMin, value);
    }

    const T &getMax() const { return vMax; }
    const T &getMin() const { return vMin; }
    const T &get() const { return value; }

    ConstrainedType<T>::Scalings scaling = LINEAR;
    float scaleFactor = 1;
    ConstrainedType<T>::Units unit = NA;

    DEFINE_TUNABLE(ConstrainedType<T>, value)
  private:
    T value, vMin, vMax;
};

// This is used to get Eigen to accept that ConstrainedType<int> can be used as an integer.
namespace Eigen
{
template <typename T>
struct NumTraits<ConstrainedType<T>> : public NumTraits<T>
{};
} // namespace Eigen
