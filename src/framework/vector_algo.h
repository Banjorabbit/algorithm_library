#pragma once
#include <vector> 

// Wrapper for std::vector that exposes the same interface as Algorithm except Process.
// https://stackoverflow.com/questions/44098116/a-c11-wrapper-class-on-top-of-std-vector
//
// author: Kristian Timm Andersen, 2019

template<typename Talgo>
class VectorAlgoStatic
{
public:

	static constexpr size_t VECTOR_ALGORITHM_VERSION_MINOR = 1; // version changes in implementation

	size_t getDynamicMemorySize() const
	{
		size_t size = 0;
		for (auto& element : vec)
		{
			size += element.getDynamicMemorySize() + element.getStaticMemorySize(); // static memory is included since it is dynamically allocated in std::vec<Talgo>
		}
		return size;
	}

	static size_t getStaticMemorySize() { return sizeof(VectorAlgoStatic<Talgo>); }

	template<typename Tcoefficients>
	bool initialize(const Tcoefficients& c)
	{
		bool flag = true;
		for (auto& element : vec) { flag &= element.initialize(c); }
		return flag;
	}

	template<typename Tcoefficients>
	bool initialize(const std::vector<Tcoefficients>& c)
	{
		bool flag = true;
		for (auto i = 0; i < c.size(); i++)
		{
			flag &= vec[i].initialize(c[i]);
		}
		return flag;
	}

	bool initialize()
	{
		bool flag = true;
		for (auto& element : vec) { flag &= element.initialize(); }
		return flag;
	}

	// get
	std::vector<typename Talgo::Coefficients> getCoefficients() const
	{
		std::vector<typename Talgo::Coefficients> cVec(vec.size());
		for (auto i = 0; i < vec.size(); i++) { cVec[i] = vec[i].getCoefficients(); }
		return cVec;
	}

	// set
	void setCoefficients(const typename Talgo::Coefficients& c) { for (size_t i = 0;i < size();i++) { vec[i].setCoefficients(c); } }

	template<typename Tcoefficients>
	void setCoefficients(const Tcoefficients& c) { for (size_t i = 0; i < size(); i++) { vec[i].setCoefficients(c[i]); } }

	auto getCoefficientsAll() const
	{
		std::vector<decltype(vec[0].getCoefficientsAll())> cVec(vec.size());
		for (size_t i = 0; i < vec.size(); i++) { cVec[i] = vec[i].getCoefficientsAll(); }
		return cVec;
	}

	template<typename Tcoefficients>
	void setCoefficientsAll(const Tcoefficients& c)
	{
		if (vec.size() != c.size()) { resize(c.size()); }
		for (size_t i = 0; i < size(); i++)
		{
			vec[i].setCoefficientsAll(c[i]);
		}
	}

	inline void resize(const size_t i) { vec = std::vector<Talgo>(i); } // algorithms have no copy or move constructor so resizing can only be done by allocating new vector
	inline size_t size() const { return vec.size(); }
	Talgo& operator[](const size_t i) { return vec[i]; }
	auto begin() { return vec.begin(); }
	auto begin() const { return vec.begin(); }
	auto end() { return vec.end(); }
	auto end()   const { return vec.end(); }

	VectorAlgoStatic() = default;
	~VectorAlgoStatic() = default;
	VectorAlgoStatic(typename Talgo::Coefficients c) { setCoefficients(c); }

protected:
	std::vector<Talgo> vec;
};

template<typename Talgo>
class VectorAlgoSimple : public VectorAlgoStatic<Talgo>
{
public:
	void reset() { for (auto& element : VectorAlgoStatic<Talgo>::vec) { element.reset(); } }

	VectorAlgoSimple() = default;
	~VectorAlgoSimple() = default;
	VectorAlgoSimple(typename Talgo::Coefficients c) : VectorAlgoStatic<Talgo>{ c } {}
};


template<typename Talgo>
class VectorAlgo
{
public:

	static constexpr size_t VECTOR_ALGORITHM_VERSION_MINOR = 1; // version changes in implementation

	size_t getDynamicMemorySize() const
	{
		size_t size = 0;
		for (auto& element : vec)
		{
			size += element.getDynamicMemorySize() + element.getStaticMemorySize(); // static memory is included since it is dynamically allocated in std::vec<Talgo>
		}
		return size;
	}

	static size_t getStaticMemorySize() { return sizeof(VectorAlgo); }

	template<typename Tcoefficients>
	bool initialize(const Tcoefficients& c)
	{
		bool flag = true;
		for (auto& element : vec) { flag &= element.initialize(c); }
		return flag;
	}

	template<typename Tcoefficients>
	bool initialize(const std::vector<Tcoefficients>& c)
	{
		bool flag = true;
		for (auto i = 0; i < c.size(); i++)
		{
			flag &= vec[i].initialize(c[i]);
		}
		return flag;
	}

	bool initialize()
	{
		bool flag = true;
		for (auto& element : vec) { flag &= element.initialize(); }
		return flag;
	}

	void reset() { for (auto& element : vec) { element.reset(); } }

	// get
	std::vector<typename Talgo::Coefficients> getCoefficients() const
	{
		std::vector<typename Talgo::Coefficients> cVec(vec.size());
		for (auto i = 0; i < vec.size(); i++) { cVec[i] = vec[i].getCoefficients(); }
		return cVec;
	}

	std::vector<typename Talgo::Parameters> getParameters() const
	{
		std::vector<typename Talgo::Parameters> pVec(vec.size());
		for (auto i = 0; i < vec.size(); i++) { pVec[i] = vec[i].getParameters(); }
		return pVec;
	}

	std::vector<typename Talgo::Setup> getSetup() const
	{
		std::vector<typename Talgo::Setup> sVec(vec.size());
		for (auto i = 0; i < vec.size(); i++) { sVec[i] = vec[i].getSetup(); }
		return sVec;
	}

	auto getCoefficientsAll() const
	{
		std::vector<decltype(vec[0].getCoefficientsAll())> cVec(vec.size());
		for (size_t i = 0; i < vec.size(); i++) { cVec[i] = vec[i].getCoefficientsAll(); }
		return cVec;
	}

	auto getParametersAll() const
	{
		std::vector<decltype(vec[0].getParametersAll())> pVec(vec.size());
		for (size_t i = 0; i < vec.size(); i++) { pVec[i] = vec[i].getParametersAll(); }
		return pVec;
	}

	auto getSetupAll() const
	{
		std::vector<decltype(vec[0].getSetupAll())> sVec(vec.size());
		for (size_t i = 0; i < vec.size(); i++) { sVec[i] = vec[i].getSetupAll(); }
		return sVec;
	}

	// set
	template<typename Tcoefficients>
	void setCoefficients(const Tcoefficients& c) { for (size_t i = 0; i < size(); i++) { vec[i].setCoefficients(c[i]); } }

	template<typename Tparameters>
	void setParameters(const Tparameters& p) { for (size_t i = 0; i < size(); i++) { vec[i].setParameters(p[i]); } }

	template<typename Tsetup>
	void setSetup(const Tsetup& setup) { for (size_t i = 0; i < size(); i++) { vec[i].setSetup(setup[i]); } }

	template<typename Tcoefficients>
	void setCoefficientsAll(const Tcoefficients& c)
	{
		if (vec.size() != c.size()) { resize(c.size()); }
		for (size_t i = 0; i < size(); i++)
		{
			vec[i].setCoefficientsAll(c[i]);
		}
	}

	template<typename Tparameters>
	void setParametersAll(const Tparameters& p)
	{
		if (vec.size() != p.size())
		{
			resize(p.size());
			uninitialize();
		}
		for (size_t i = 0; i < size(); i++) { vec[i].setParametersAll(p[i]); }
	}
	template<typename Tsetup>
	void setSetupAll(const Tsetup& s) { resize(s.size()); for (size_t i = 0; i < size(); i++) { vec[i].setSetupAll(s[i]); } }

	void disable() { for (auto& element : vec) { element.disable(); } }
	void enable() { for (auto& element : vec) { element.enable(); } }
	void uninitialize() { for (auto& element : vec) { element.uninitialize(); } }

	inline void resize(const size_t i) { vec = std::vector<Talgo>(i); } // algorithms have no copy or move constructor so resizing can only be done by allocating new vector
	inline size_t size() const { return vec.size(); }
	Talgo& operator[](const size_t i) { return vec[i]; }
	auto begin() { return vec.begin(); }
	auto begin() const { return vec.begin(); }
	auto end() { return vec.end(); }
	auto end()   const { return vec.end(); }

	auto getInitialized() const
	{
		std::vector<decltype(Talgo::getInitialized())> iVec(vec.size());
		for (auto i = 0; i < vec.size(); i++) { iVec[i] = vec[i].initialized(); }
		return iVec;
	}

	auto getEnabled() const
	{
		std::vector<decltype(Talgo::getEnabled())> gVec(vec.size());
		for (auto i = 0; i < vec.size(); i++) { gVec[i] = vec[i].getEnabled(); }
		return gVec;
	}

	static constexpr size_t STATIC_VECTOR_ALGO_VERSION = 1; // version changes in ABI
private:
	std::vector<Talgo> vec;
};
