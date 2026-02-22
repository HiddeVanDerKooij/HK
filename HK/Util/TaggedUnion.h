// Copyright (c) 2023-2024, Hidde van der Kooij
// SPDX-License-Identifier: BSD-2-Clause

#pragma once

#include "Common/Types.h"
#include "Common/CompilerMacros.h"
#include "Common/Meta.h"

// This class stores N types in a union.
// It has a tag T that indicates which type is currently active.
// The tag is used to determine which type to access.

template<typename T, typename... Types>
class TaggedUnion {
public:
	static_assert(Meta::GetNumTypes<Types...>() > 0, "At least one type must be provided");
	static_assert(Meta::GetNumTypes<Types...>() <= Traits::Limits<T>::Max, "Too many types, use a larger tag type");
	TaggedUnion() : TypeTag(0) {}

	// Set the tag and value
	template<typename U>
	void Set(U value) {
		TypeTag = GetTag<U>();
		*(U*)(&Data) = U(value);
	}
	
	template<typename U>
	bool Get(U& value) const {
		constexpr T tag = GetTag<U>();
		if (tag == TypeTag) {
			value = *reinterpret_cast<const U*>(&Data);
			return true;
		}
		return false;
	}
	
	// Assumes you never mutate the data as it doesn't invalidate the pointer
	// while it should. But this enables more elegant user
	// syntax so this is still a welcome trade-off.
	template<typename U>
	const U* GetAs() const {
		constexpr T tag = GetTag<U>();
		if (tag == TypeTag) {
			return reinterpret_cast<const U*>(&Data);
		}
		return nullptr;
	}
	
	template<typename U>
	U& GetChecked() {
		constexpr T tag = GetTag<U>();
		if (tag != TypeTag) {
			UNREACHABLE;
		}
		return *reinterpret_cast<U*>(&Data);
	}
	
	template<typename U>
	const U& GetChecked() const {
		constexpr T tag = GetTag<U>();
		if (tag != TypeTag) {
			UNREACHABLE;
		}
		return *reinterpret_cast<const U*>(&Data);
	}
	
	template<typename U>
	bool IsType() const {
		constexpr T tag = GetTag<U>();
		return tag == TypeTag;
	}
	
	const T& GetType() const {
		return TypeTag;
	}

	template<typename U>
	static constexpr T GetTag() {
		constexpr int32 index = Meta::GetIndex<U, Types...>();
		static_assert(index != -1, "Type not supported");
		return static_cast<T>(index);
	}
	
private:
	T TypeTag;
	uint8 Data[Meta::GetMaxSize<Types...>::Value];
};