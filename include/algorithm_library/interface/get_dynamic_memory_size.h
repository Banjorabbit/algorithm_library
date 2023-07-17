// Member function added to Eigen DenseBase class to get dynamic memory size of array and matrices
//
// author: Kristian Timm Andersen, 2019

inline size_t getDynamicMemorySize() const
{
	if (MaxRowsAtCompileTime == Dynamic || MaxColsAtCompileTime == Dynamic)
	{
		return sizeof(DenseBase<Derived>::Scalar)*this->size();
	}
	else
	{
		return size_t(0);
	}
	
}
