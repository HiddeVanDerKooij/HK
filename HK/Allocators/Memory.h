// Copyright (c) 2023-2024, Hidde van der Kooij
// SPDX-License-Identifier: BSD-2-Clause

#pragma once

#include "Common/CompilerMacros.h"
#include "Common/Types.h"

// We define placement new ourselves, to avoid dragging in many
// other header files. However it is possibly already defined.
#ifdef GCC
#ifndef _NEW
#define _NEW
#define _HK_DEFINE_PLACEMENTNEW
#endif
#endif

#ifdef _HK_DEFINE_PLACEMENTNEW
inline void* operator new(size64, void* p) noexcept {
   	return p;
}
inline void operator delete(void*, void*) noexcept {}
#endif

class StringView;

template<typename T>
struct RemoveReference {
	typedef T Type;
};

template<typename T>
struct RemoveReference<T&> {
	typedef T Type;
};

template<typename T>
struct RemoveReference<T&&> {
	typedef T Type;
};

template<typename T>
typename RemoveReference<T>::Type&& Move(T&& arg) noexcept {
	return static_cast<typename RemoveReference<T>::Type&&>(arg);
}

template<typename T>
[[__nodiscard__]]
T&& Forward(typename RemoveReference<T>::Type& arg) noexcept {
	return static_cast<T&&>(arg);
}

template<typename T>
[[__nodiscard__]]
T&& Forward(typename RemoveReference<T>::Type&& arg) noexcept {
	return static_cast<T&&>(arg);
}

namespace Memory {
	void* Allocate(uint64 numBytes);
	
	template<typename T>
	T* Allocate(uint64 numElements) {
		return (T*)Allocate(numElements * sizeof(T));
	};
	
	void Reallocate(void*& ptr, uint64 oldNumBytes, uint64 newNumBytes);
	void Free(void* ptr, uint64 numBytes);
	void FillZero(void* Start, uint64 numBytes);
	void FillByte(void* Start, uint64 numBytes, uint8 value);

	template<typename T, typename... P>
	T* PlacementNew(void* ptr, P&&... args) {
		return (T*)(new(ptr) T(Forward<P>(args)...));
	}

	template<typename T, typename... P>
	T* New(P&&... args) {

		// TODO (HvdK): Find all the nuance of std::forward, and make
		// sure I use this correctly. Perhaps implement our own std::forward.
		//T* ptr = (T*)Allocate(sizeof(T));
		//new (ptr) T(static_cast<P&&>(args)...);
		//return ptr;

		// TODO (HvdK): Use the snippet above instead of this one.
		return new T(Forward<P>(args)...);
	};

	template<typename T>
	void Delete(T* ptr) {
		ptr->~T();
		Free(ptr, sizeof(T));
	};

	void Move(const void* from, void* to, uint64 numBytes);
	void Copy(const void* from, void* to, uint64 numBytes);
	template<typename T>
	void Copy(const T* from, T* to) {
		Copy(from, to, sizeof(T));
	};
	
	bool Equals(const void* a, const void* b, uint64 numBytes);
	// TODO (HvdK): Make special aligned versions of these functions
	bool StringEquals(const char8* a, const char8* b);
	bool StringEquals(const char8* a, const char8* b, uint32 size);
	bool StringEquals(const StringView& a, const StringView& b);
	uint32 StringSize(const char8* string);
};

template<typename T>
void Swap(T& a, T& b) {
	T temp = Move(a);
	a = Move(b);
	b = Move(temp);
}
