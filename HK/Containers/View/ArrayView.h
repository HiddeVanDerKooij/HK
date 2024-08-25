#pragma once

#include "Common/Types.h"

template<typename T> class Array;

class GArrayView
{
public:
	GArrayView() = default;
	GArrayView(const void* data, uint32 size);

	uint32 Size() const;
protected:
	void CheckIndex(uint32 index) const;

	const void* ArrayData;
private:
	uint32 ArraySize;
};

template<typename T>
class ArrayView : public GArrayView
{
public:
	ArrayView() = default;
	ArrayView(const T* data, uint32 size) : GArrayView(data, size) {}
	ArrayView(const ArrayView<T>& other) : GArrayView(other.ArrayData, other.ArraySize) {}
	ArrayView(const Array<T>& source) : GArrayView(source.GetData(), source.Num()) {}

	const T& operator[](uint32 index) const { CheckIndex(index);  return ConstData()[index]; }
	const T* ConstData() const { return static_cast<const T*>(ArrayData); }
};