// Copyright (c) 2023-2024, Hidde van der Kooij
// SPDX-License-Identifier: BSD-2-Clause

#pragma once

#include "Allocators/Memory.h"
#include "Containers/Array.h"
#include "Common/CompilerMacros.h"
#include "Common/Types.h"
#include "Containers/View/StringView.h"

template<int S, int N>
class GPool {
public:
	GPool() = default;
	GPool(GPool&) = delete;
	GPool(GPool&&) = default;
	~GPool();
protected:
	uint8* Allocate_Internal();
	void Free_Internal(uint8* ptr);

private:
	void AllocateNewBuffer();

	Array<uint8*> Buffers;
	Array<uint8*> FreeList;
};

template<int S, int N>
GPool<S,N>::~GPool() {
	for (uint32 i = 0; i < Buffers.Num(); ++i) {
		Memory::Free(Buffers[i], S * N);
	}
}

template<int S, int N>
uint8* GPool<S,N>::Allocate_Internal() {
	if (UNLIKELY(FreeList.Num() == 0))
	{
		AllocateNewBuffer();
	}

	return FreeList.Pop();
}

template<int S, int N>
void GPool<S,N>::Free_Internal(uint8* ptr) {
	FreeList.Add(ptr);
}

template<int S, int N>
void GPool<S, N>::AllocateNewBuffer() {
	uint8* buffer = (uint8*)Memory::Allocate(S*N);
	Buffers.Add(buffer);

	for (uint32 i = 0; i < N; ++i) {
		FreeList.Add(buffer + i * S);
	}
}


template<typename T>
class Pool : public GPool<sizeof(T), 1024> {
public:
	T* Allocate();
	void Free(T* ptr);
};

template<typename T>
T* Pool<T>::Allocate() {
	return reinterpret_cast<T*>(this->Allocate_Internal());
}

template<typename T>
void Pool<T>::Free(T* ptr) {
	this->Free_Internal(reinterpret_cast<uint8*>(ptr));
}