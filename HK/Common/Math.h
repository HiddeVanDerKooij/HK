// Copyright (c) 2023-2024, Hidde van der Kooij
// SPDX-License-Identifier: BSD-2-Clause

#pragma once

#include "Common/Types.h"

namespace Math {
	template<typename T>
	T Min(T a, T b) {
		return a < b ? a : b;
	};
	template<typename T>
	T Max(T a, T b) {
		return a > b ? a : b;
	};
	
	template<typename T>
	typename Traits::MaskFor<T>::Type GetSignBitMask() {
		static_assert(Traits::IsSigned<T>::Value);
		return 1 << (sizeof(T)*8-1);
	};
	template<typename T>
	typename Traits::MaskFor<T>::Type GetSignBit(T a) {
		static_assert(Traits::IsSigned<T>::Value);
		return Traits::MaskFor<T>::Convert(a) & GetSignBitMask<T>();
	};
	template<typename T>
	T Abs(T a) {
		static_assert(Traits::IsSigned<T>::Value);
		
		typename Traits::MaskFor<T>::Type value = Traits::MaskFor<T>::Convert(a);
		value &= ~GetSignBitMask<T>();
		return *reinterpret_cast<T*>(&value);
	};
	
	namespace {
		template <typename T, typename O, bool IsSignedType = Traits::IsSigned<T>::Value>
		struct RoundToIntImpl;
		
		template<typename T, typename O>
		struct RoundToIntImpl<T, O, true> {
			static O Exec(T a) {
				T sign = GetSignBit(a);
				T value = Abs(a) + T(0.5);
				O output = O(value);
				if (!!sign) {
					return -output;
				}
				return output;
			};
		};
		
		template<typename T, typename O>
		struct RoundToIntImpl<T, O, false> {
			static O Exec(T a) {
				T value = a + T(0.5);
				O output = O(value);
				return output;
			};
		};
	}
	
	template<typename O, typename T>
	O RoundToInt(T a) {
		return RoundToIntImpl<T, O>::Exec(a);
	};
}