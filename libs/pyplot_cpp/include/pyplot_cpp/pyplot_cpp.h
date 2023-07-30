#pragma once
#include <string>
#include <array>
#include "algorithm_library/interface/input_output.h"

namespace Pyplotcpp
{
    void plot(I::Real2D x);
    void plot(I::Real2D x, I::Real2D y);

    void imagesc(I::Real2D x);
    void imagesc(I::Real2D x, std::array<float, 2> scaling);

    void title(const std::string& titlename);
    void xlim(float left, float right);
    void ylim(float down, float up);
    void xlabel(const std::string& label);
    void ylabel(const std::string& label);

    void save(const std::string& filename, const int dpi = 0);
    void show(const bool block = true);
    void close();
}