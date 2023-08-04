#include "pyplot_cpp/pyplot_cpp.h"
#include "matplotlibcpp.h"

namespace Pyplotcpp
{
    void plot(I::Real2D x)
    {
        const int nCols = x.cols();
        const int nRows = x.rows();
        for (auto col = 0; col < nCols; col++)
        {
            const float *xPtr = x.col(col).data();
            std::vector<float> xVec(xPtr, xPtr + nRows);
            matplotlibcpp::plot(xVec);
        }
    }

    void plot(I::Real2D x, I::Real2D y)
    {
        const int nCols = std::min(x.cols(), y.cols());
        const int nRows = std::min(x.rows(), y.rows());
        for (auto col = 0; col < nCols; col++)
        {
            const float *xPtr = x.col(col).data();
            const float *yPtr = y.col(col).data();
            std::vector<float> xVec(xPtr, xPtr + nRows);
            std::vector<float> yVec(yPtr, yPtr + nRows);
            matplotlibcpp::plot(xVec, yVec);
        }
    }

    // local function 
    inline Figure* imshow(I::Real2D x, const std::map<std::string, std::string> &options = {})
    {
        const int nCols = x.cols();
        const int nRows = x.rows();
        const int colors = 1;
        Eigen::ArrayXXf y = x.transpose(); // matplotlib assumes data is transposed
        PyObject* figure;
        matplotlibcpp::imshow(y.data(), nRows, nCols, colors, options, &figure);
        return reinterpret_cast<Figure*>(figure);
    }
    
    Figure* imagesc(I::Real2D x)
    {
        return imshow(x);
    }

    Figure* imagesc(I::Real2D x, std::array<float, 2> scaling)
    {
        const std::map<std::string, std::string> sc{{"vmin", std::to_string(scaling[0])}, {"vmax", std::to_string(scaling[1])}};
        return imshow(x, sc);
    }

    void title(const std::string& titlename)
    {
        matplotlibcpp::title(titlename);
    }

    void xlim(float left, float right)
    {
        matplotlibcpp::xlim(left, right);
    }

    void ylim(float down, float up)
    {
        matplotlibcpp::ylim(down, up);
    }

    void xlabel(const std::string& label)
    {
        matplotlibcpp::xlabel(label);
    }

    void ylabel(const std::string& label)
    {
        matplotlibcpp::ylabel(label);
    }

    void colorbar(Figure* mat)
    {
        matplotlibcpp::colorbar(reinterpret_cast<PyObject*>(mat));
    }
    
    void save(const std::string& filename, const int dpi)
    {
        matplotlibcpp::save(filename, dpi);
    }

    void show(const bool block)
    {
        matplotlibcpp::show(block);
    }

    void close()
    {
        matplotlibcpp::close();
    }
}