#pragma once
#include <vector>

// Wrapper for std::vector that exposes the same interface as Algorithm except Process.
// https://stackoverflow.com/questions/44098116/a-c11-wrapper-class-on-top-of-std-vector
//
// author: Kristian Timm Andersen, 2019

template <typename Talgo>
class VectorAlgo
{
  public:
    static constexpr size_t VECTOR_ALGORITHM_VERSION_MINOR = 1; // version changes in implementation

    using Coefficients = typename Talgo::Coefficients;
    using Parameters = typename Talgo::Parameters;
    using Setup = typename Talgo::Setup;

    VectorAlgo() = default;
    VectorAlgo(int size) : vec(size) {}                                         // initialize with size
    VectorAlgo(int size, const Coefficients &c) : vec(size, c) {}               // intitialize with size and Coefficients c
    VectorAlgo(int size, const Setup &s) : vec(size, s) {}                      // initialize with size and Setup S
    VectorAlgo(const std::vector<Coefficients> &c) : vec(c.begin(), c.end()) {} // initialize with vector of Coefficients
    VectorAlgo(const std::vector<Setup> &s) : vec(s.begin(), s.end()) {}        // initialize with vector of Setups
    virtual ~VectorAlgo() = default;

    size_t getDynamicSize() const
    {
        size_t size = 0;
        for (auto &element : vec)
        {
            size += element.getDynamicSize() + element.getStaticSize(); // static memory is included since it is dynamically allocated in std::vec<Talgo>
        }
        return size;
    }

    static size_t getStaticSize() { return sizeof(VectorAlgo); }

    void reset()
    {
        for (auto &element : vec)
        {
            element.reset();
        }
    }

    bool isConfigurationValid() const
    {
        bool flag = true;
        for (auto &element : vec)
        {
            flag &= element.isConfigurationValid();
        }
        return flag;
    }

    // get
    std::vector<Coefficients> getCoefficients() const
    {
        std::vector<Coefficients> cVec(vec.size());
        for (auto i = 0; i < static_cast<int>(vec.size()); i++)
        {
            cVec[i] = vec[i].getCoefficients();
        }
        return cVec;
    }

    std::vector<Parameters> getParameters() const
    {
        std::vector<Parameters> pVec(vec.size());
        for (auto i = 0; i < vec.size(); i++)
        {
            pVec[i] = vec[i].getParameters();
        }
        return pVec;
    }

    std::vector<Setup> getSetup() const
    {
        std::vector<Setup> sVec(vec.size());
        for (auto i = 0; i < vec.size(); i++)
        {
            sVec[i] = vec[i].getSetup();
        }
        return sVec;
    }

    auto getCoefficientsTree() const
    {
        std::vector<decltype(vec[0].getCoefficientsTree())> cVec(vec.size());
        for (size_t i = 0; i < vec.size(); i++)
        {
            cVec[i] = vec[i].getCoefficientsTree();
        }
        return cVec;
    }

    auto getParametersTree() const
    {
        std::vector<decltype(vec[0].getParametersTree())> pVec(vec.size());
        for (size_t i = 0; i < vec.size(); i++)
        {
            pVec[i] = vec[i].getParametersTree();
        }
        return pVec;
    }

    auto getSetupTree() const
    {
        std::vector<decltype(vec[0].getSetupTree())> sVec(vec.size());
        for (size_t i = 0; i < vec.size(); i++)
        {
            sVec[i] = vec[i].getSetupTree();
        }
        return sVec;
    }

    // set
    void setCoefficients(const Coefficients &c)
    {
        for (size_t i = 0; i < size(); i++)
        {
            vec[i].setCoefficients(c);
        }
    }

    template <typename Tcoefficients>
    void setCoefficients(const Tcoefficients &c)
    {
        if (c.size() != size()) { resize(c.size()); }
        for (size_t i = 0; i < size(); i++)
        {
            vec[i].setCoefficients(c[i]);
        }
    }

    void setParameters(const Parameters &p)
    {
        for (size_t i = 0; i < size(); i++)
        {
            vec[i].setParameters(p);
        }
    }

    template <typename Tparameters>
    void setParameters(const Tparameters &p)
    {
        if (p.size() != size()) { resize(p.size()); }
        for (size_t i = 0; i < size(); i++)
        {
            vec[i].setParameters(p[i]);
        }
    }

    void setSetup(const Setup &s)
    {
        for (size_t i = 0; i < size(); i++)
        {
            vec[i].setSetup(s);
        }
    }

    template <typename Tsetup>
    void setSetup(const Tsetup &s)
    {
        if (s.size() != size()) { resize(s.size()); }
        for (size_t i = 0; i < size(); i++)
        {
            vec[i].setSetup(s[i]);
        }
    }

    template <typename Tcoefficients>
    void setCoefficientsTree(const Tcoefficients &c)
    {
        if (vec.size() != c.size()) { resize(c.size()); }
        for (size_t i = 0; i < size(); i++)
        {
            vec[i].setCoefficientsTree(c[i]);
        }
    }

    template <typename Tparameters>
    void setParametersTree(const Tparameters &p)
    {
        if (vec.size() != p.size()) { resize(p.size()); }
        for (size_t i = 0; i < size(); i++)
        {
            vec[i].setParametersTree(p[i]);
        }
    }

    template <typename Tsetup>
    void setSetupTree(const Tsetup &s)
    {
        if (s.size() != size()) { resize(s.size()); }
        for (size_t i = 0; i < size(); i++)
        {
            vec[i].setSetupTree(s[i]);
        }
    }

    inline void resize(const size_t i)
    {
        vec = std::vector<Talgo>(i);
    } // algorithms might not have a copy or move constructor so resizing can only be done by allocating new vector
    inline size_t size() const { return vec.size(); }
    Talgo &operator[](const size_t i) { return vec[i]; }
    const Talgo &operator[](const size_t i) const { return vec[i]; }
    auto begin() { return vec.begin(); }
    auto begin() const { return vec.begin(); }
    auto end() { return vec.end(); }
    auto end() const { return vec.end(); }

  private:
    std::vector<Talgo> vec;
};
