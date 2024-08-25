#pragma once

// TODO (HvdK): Find a way to avoid including this header
#include <new>

#include "Allocators/Memory.h"
#include "Common/CompilerMacros.h"
#include "Common/Types.h"
#include "Containers/View/ArrayView.h"

// TODO (HvdK): Reduce code generation by having a templated
// array implementation that only worries about sizeof(T),
// so that different Ts with the same size can reuse.

static uint32 CalculateArrayMaxGrowth(uint32 oldcap, uint32 newcap)
{
	oldcap <<= 1;
	if (oldcap >= newcap) {
		return oldcap;
	}
	return newcap;
};


template<typename T>
class Array {
public:
	Array();
	Array(uint32 buffer);
	Array(const Array& other);
	Array(Array&& other);
	Array(ArrayView<T> other);
	~Array();

	void Add(const T& item);
	void Add(T&& item);
	T& AddRef(const T& item);
	T& AddRef(T&& item);
	T& AddDefaulted();
	// Adds uninitialized elements and returns the
	// pointer to the first one.
	T* AddUninitialized(uint32 num);
	void AddRange(const ArrayView<T>& items);
	void RemoveAt(uint32 index);
	void RemoveAtSwap(uint32 index);
	T&& Pop();
	uint32 Num() const;
	void Reserve(uint32 num);
	// Requires an exact size
	void RequireArrayMax(uint32 cap);
	// Requires an exact size, but grow exponentially
	void RequireArrayMaxGrowth(uint32 cap);
	// Resets the array to an empty state,
	// doesn't change any allocations.
	void Reset();
	void Reverse();

	static const uint64 ElementSize = sizeof(T);

	bool IsValidIndex(uint32 index) const;

	T& operator[](uint32 index);
	const T& operator[](uint32 index) const;

	Array<T>& operator=(const Array<T>& other);
	Array<T>& operator=(Array<T>&& other);

	T* GetData();
	const T* GetData() const;

private:
	void InitEmpty();

	void Allocate(uint32 cap);
	void Reallocate(uint32 cap);
	void Free();
protected:
	T* Data;
	uint32 ArrayNum;
	uint32 ArrayMax;
};

template<typename T>
Array<T>::Array()
{
	InitEmpty();
}

template<typename T>
Array<T>::Array(uint32 buffer)
{
	if (buffer > 0) {
		Data = nullptr;
		ArrayNum = 0;
		Allocate(buffer);
	}
	else {
		InitEmpty();
	}
}

template<typename T>
Array<T>::Array(const Array& other)
{
	if (other.ArrayNum > 0) {
		Data = nullptr;
		Allocate(other.ArrayNum);
		ArrayNum = other.ArrayNum;
		Memory::Copy(other.Data, Data, ElementSize * ArrayNum);
	}
	else {
		InitEmpty();
	}
}

template<typename T>
Array<T>::Array(Array&& other)
{
	Data = other.Data;
	ArrayNum = other.ArrayNum;
	ArrayMax = other.ArrayMax;

	other.InitEmpty();
}

template<typename T>
Array<T>::Array(ArrayView<T> other)
{
	uint32 num = other.Size();
	if (num > 0) {
		Data = nullptr;
		Allocate(num);
		ArrayNum = num;
		Memory::Copy(other.ConstData(), Data, ElementSize * num);
	}
	else {
		InitEmpty();
	}
}

template<typename T>
Array<T>::~Array()
{
	if (LIKELY(Data != nullptr))
		Free();
}

template<typename T>
void Array<T>::Add(const T& item)
{
	RequireArrayMaxGrowth(ArrayNum + 1);
	new (&Data[ArrayNum++]) T(item);
}

template<typename T>
void Array<T>::Add(T&& item)
{
	RequireArrayMaxGrowth(ArrayNum + 1);
	new (&Data[ArrayNum++]) T(Move(item));
}

template<typename T>
void Array<T>::Reset()
{
	// TODO (HvdK): Consider running destructors
	ArrayNum = 0;
}

template<typename T>
void Array<T>::Reverse()
{
	uint32 n = ArrayNum >> 1;
	for (uint32 i = 0; i < n; ++i) {
		T& a = Data[i];
		T& b = Data[ArrayNum - i - 1];
		T tmp = Move(a);
		a = Move(b);
		b = Move(tmp);
	}
}

template<typename T>
T& Array<T>::AddRef(const T& item)
{
	Add(item);
	return Data[ArrayNum - 1];
}

template<typename T>
T& Array<T>::AddRef(T&& item)
{
	Add(Move(item));
	return Data[ArrayNum - 1];
}

template<typename T>
T& Array<T>::AddDefaulted()
{
	T v = T();
	return AddRef(Move(v));
}

template<typename T>
[[nodiscard]] T* Array<T>::AddUninitialized(uint32 num)
{
	RequireArrayMaxGrowth(ArrayNum + num);
	T* result = &Data[ArrayNum];
	ArrayNum += num;
	return result;
}

template<typename T>
void Array<T>::RemoveAt(uint32 index)
{
	CHECK(IsValidIndex(index));

	--ArrayNum;

	if (LIKELY(index < ArrayNum)) {
		int32 numElementsToMove = ArrayNum - index;
		Memory::Move(&Data[index + 1], &Data[index], ElementSize * numElementsToMove);
	}
}

template<typename T>
void Array<T>::RemoveAtSwap(uint32 index)
{
	CHECK(IsValidIndex(index));

	--ArrayNum;

	if (LIKELY(index < ArrayNum)) {
		Data[index] = Data[ArrayNum];
	}
}

template<typename T>
T&& Array<T>::Pop()
{
	CHECK(ArrayNum > 0);
	return Move(Data[--ArrayNum]);
}

template<typename T>
uint32 Array<T>::Num() const
{
	return ArrayNum;
}

template<typename T>
void Array<T>::Reserve(uint32 num)
{
	RequireArrayMaxGrowth(ArrayNum + num);
}

template<typename T>
void Array<T>::RequireArrayMax(uint32 newcap)
{
	if (ArrayMax < newcap) {
		if (UNLIKELY(Data == nullptr)) {
			Allocate(newcap);
		} else {
			Reallocate(newcap);
		}
	}
}

template<typename T>
void Array<T>::RequireArrayMaxGrowth(uint32 newcap)
{
	if (ArrayMax < newcap) {
		RequireArrayMax(CalculateArrayMaxGrowth(ArrayMax, newcap));
	}
}

template<typename T>
bool Array<T>::IsValidIndex(uint32 index) const
{
	return index < Num();
}

template<typename T>
T& Array<T>::operator[](uint32 index)
{
	CHECK(IsValidIndex(index));
	return Data[index];
}

template<typename T>
const T& Array<T>::operator[](uint32 index) const
{
	CHECK(IsValidIndex(index));
	return Data[index];
}

template<typename T>
Array<T>& Array<T>::operator=(const Array<T>& other)
{
	CHECK(this != &other);
	if (other.ArrayNum > 0) {
		RequireArrayMax(other.ArrayNum);
		ArrayNum = other.ArrayNum;
		Memory::Copy(other.Data, Data, ElementSize * ArrayNum);
	} else {
		ArrayNum = 0;
	}
	return *this;
}

template<typename T>
Array<T>& Array<T>::operator=(Array<T>&& other)
{
	CHECK(this != &other);
	if (LIKELY(Data != nullptr))
		Free();

	Data = other.Data;
	ArrayNum = other.ArrayNum;
	ArrayMax = other.ArrayMax;

	other.InitEmpty();
	return *this;
}

template<typename T>
T* Array<T>::GetData()
{
	return Data;
}

template<typename T>
const T* Array<T>::GetData() const
{
	return Data;
}

template<typename T>
void Array<T>::InitEmpty()
{
	Data = nullptr;
	ArrayNum = 0;
	ArrayMax = 0;
}


template<typename T>
void Array<T>::Allocate(uint32 newcap)
{
	CHECK(Data == nullptr);
	ArrayMax = newcap;
	Data = (T*)Memory::Allocate(newcap * ElementSize);
}

template<typename T>
void Array<T>::Reallocate(uint32 newcap)
{
	CHECK(Data != nullptr);
	uint64 oldsize = ElementSize * ArrayMax;
	uint64 newsize = ElementSize * newcap;

	void* data = Data;
	Memory::Reallocate(data, oldsize, newsize);
	Data = (T*)data;
	ArrayMax = newcap;
}

template<typename T>
void Array<T>::Free()
{
	CHECK(Data != nullptr);
	Memory::Free(Data, ArrayMax * ElementSize);
}