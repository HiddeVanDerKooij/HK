// Copyright (c) 2023-2024, Hidde van der Kooij
// SPDX-License-Identifier: BSD-2-Clause

#pragma once

// TODO (HvdK): G++ wants this file here
#include <stddef.h>

typedef long long int64;
typedef unsigned long long uint64;
typedef int int32;
typedef unsigned int uint32;
typedef short int16;
typedef unsigned short uint16;
typedef char int8;
typedef unsigned char uint8;
typedef float f32;
typedef double f64;

typedef int8 char8;

typedef size_t size64;
typedef uint64 uintptr;

constexpr int8 LIMIT_INT8_MIN = -128;
constexpr int8 LIMIT_INT8_MAX = 127;
constexpr uint8 LIMIT_UINT8_MAX = 255;
constexpr int16 LIMIT_INT16_MIN = -32768;
constexpr int16 LIMIT_INT16_MAX = 32767;
constexpr uint16 LIMIT_UINT16_MAX = 65535;
constexpr int32 LIMIT_INT32_MIN = -2147483648;
constexpr int32 LIMIT_INT32_MAX = 2147483647;
constexpr uint32 LIMIT_UINT32_MAX = 4294967295U;
constexpr int64 LIMIT_INT64_MIN = -9223372036854775807LL - 1;
constexpr int64 LIMIT_INT64_MAX = 9223372036854775807LL;
constexpr uint64 LIMIT_UINT64_MAX = 18446744073709551615ULL;

constexpr uint32 INVALID_INDEX = 0xFFFFFFFF;

enum ENoInit { NoInit };

typedef char8 nil;

namespace Traits {

template<typename T>
struct IsSigned {
	static constexpr bool Value = false;
};
struct _Signed {
	static constexpr bool Value = true;
};
template<> struct IsSigned<int64> : public _Signed {};
template<> struct IsSigned<int32> : public _Signed {};
template<> struct IsSigned<int16> : public _Signed {};
template<> struct IsSigned<int8> : public _Signed {};
template<> struct IsSigned<f32> : public _Signed {};
template<> struct IsSigned<f64> : public _Signed {};

// A type definition for any pod to an unsigned integer of the same size
template<int Size>
struct UnsignedOfSize {};
template<> struct UnsignedOfSize<1> { typedef uint8 Type; };
template<> struct UnsignedOfSize<2> { typedef uint16 Type; };
template<> struct UnsignedOfSize<4> { typedef uint32 Type; };
template<> struct UnsignedOfSize<8> { typedef uint64 Type; };

// Convert a pod to one of equivalent sized unsigned integer
template<typename T>
struct MaskFor {
	typedef typename UnsignedOfSize<sizeof(T)>::Type Type;
	static Type Convert(T a) {
		Type output;
		output = *reinterpret_cast<Type*>(&a);
		return output;
	};
};
};