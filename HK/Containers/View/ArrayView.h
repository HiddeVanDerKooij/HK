// Copyright (c) 2023-2024, Hidde van der Kooij
// SPDX-License-Identifier: BSD-2-Clause

#pragma once

#include "Common/Types.h"

template<typename T> class Array;

class GArrayView
{
public:
	GArrayView() = default;
	GArrayView(const void* data, uint32 size);

	uint32 Size() const;
	void Reset();
protected:
	void CheckIndex(uint32 index) const;

	const void* ArrayData;
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
	
	ArrayView<T> ChopLeft(uint32 count) const { CHECK(count > 0); CheckIndex(count - 1); return ArrayView<T>(&ConstData()[count], ArraySize - sizeof(T)*count); }
	ArrayView<T> ChopRight(uint32 count) const { CheckIndex(count); return ArrayView<T>(ConstData(), ArraySize - sizeof(T)*count); }

	const T& operator[](uint32 index) const { CheckIndex(index);  return ConstData()[index]; }
	const T* ConstData() const { return static_cast<const T*>(ArrayData); }
};