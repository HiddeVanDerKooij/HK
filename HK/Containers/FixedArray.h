// Copyright (c) 2024, Hidde van der Kooij
// SPDX-License-Identifier: BSD-2-Clause

#pragma once

#include "Common/CompilerMacros.h"
#include "Common/Types.h"
#include "Containers/View/ArrayView.h"
#include "Algo/Sort.h"

template<typename T, int N>
class FixedArray {
	static_assert(N > 0, "FixedArray size must be greater than 0");
public:
	FixedArray();
	FixedArray(const FixedArray<T, N>& other);
	FixedArray(FixedArray<T, N>&& other);
	
	~FixedArray();
	
	template<typename... Args>
	FixedArray(const Args&... args) {
		static_assert(sizeof...(args) <= N, "FixedArray size must match or exceed the number of arguments");
		CreateFrom(0, args...);
	};
	
	template<typename Arg, typename... Args>
	void CreateFrom(uint32 index, const Arg& arg, const Args&... args) {
		CHECK(index < N);
		Data[index] = arg;
		CreateFrom(index + 1, args...);
	};
	
	template<typename Arg>
	void CreateFrom(uint32 index, const Arg& arg) {
		CHECK(index < N);
		Data[index] = arg;
	};
	
	FixedArray<T, N>& operator=(const FixedArray<T, N>& other);
	T* begin() { return Data; }
	T* end() { return Data + N; }
	const T* begin() const { return Data; }
	const T* end() const { return Data + N; }
	
	T* operator*() { return Data; }
	
	T& operator[](uint32 index);
	const T& operator[](uint32 index) const;
	T& At(uint32 index);
	const T& At(uint32 index) const;
	
	uint32 Size() const;
	
	T Data[N];
};

template<typename T, int N>
FixedArray<T, N>::FixedArray()
{
}

template<typename T, int N>
FixedArray<T, N>::FixedArray(const FixedArray<T, N>& other)
{
	for (uint32 i = 0; i < N; ++i)
	{
		Data[i] = other.Data[i];
	}
}

template<typename T, int N>
FixedArray<T, N>::FixedArray(FixedArray<T, N>&& other)
{
	for (uint32 i = 0; i < N; ++i)
	{
		Data[i] = Move(other.Data[i]);
	}
}

template<typename T, int N>
FixedArray<T, N>::~FixedArray()
{
}

template<typename T, int N>
FixedArray<T, N>& FixedArray<T, N>::operator=(const FixedArray<T, N>& other)
{
	for (uint32 i = 0; i < N; ++i)
	{
		Data[i] = other.Data[i];
	}
	return *this;
}

template<typename T, int N>
T& FixedArray<T, N>::operator[](uint32 index)
{
	return At(index);
}

template<typename T, int N>
const T& FixedArray<T, N>::operator[](uint32 index) const
{
	return At(index);
}

template<typename T, int N>
T& FixedArray<T, N>::At(uint32 index)
{
	CHECK(index < N);
	return Data[index];
}

template<typename T, int N>
const T& FixedArray<T, N>::At(uint32 index) const
{
	CHECK(index < N);
	return Data[index];
}

template<typename T, int N>
uint32 FixedArray<T, N>::Size() const
{
	return N;
}