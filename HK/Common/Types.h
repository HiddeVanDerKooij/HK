// Copyright (c) 2023-2024, Hidde van der Kooij
// SPDX-License-Identifier: BSD-2-Clause

#pragma once

// TODO (HvdK): G++ wants this file here
#include <stddef.h>

// Some forward declarations
class StringView;


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

template<typename T>
struct Limits {
	static constexpr T Min = 0;
	static constexpr T Max = 0;
};
template<> struct Limits<int8> {
	static constexpr int8 Min = -128;
	static constexpr int8 Max = 127;
};
template<> struct Limits<uint8> {
	static constexpr uint8 Min = 0;
	static constexpr uint8 Max = 255;
};
template<> struct Limits<int16> {
	static constexpr int16 Min = -32768;
	static constexpr int16 Max = 32767;
};
template<> struct Limits<uint16> {
	static constexpr uint16 Min = 0;
	static constexpr uint16 Max = 65535;
};
template<> struct Limits<int32> {
	static constexpr int32 Min = -2147483647 - 1;
	static constexpr int32 Max = 2147483647;
};
template<> struct Limits<uint32> {
	static constexpr uint32 Min = 0;
	static constexpr uint32 Max = 4294967295;
};
template<> struct Limits<int64> {
	static constexpr int64 Min = -9223372036854775807 - 1;
	static constexpr int64 Max = 9223372036854775807;
};
template<> struct Limits<uint64> {
	static constexpr uint64 Min = 0;
	static constexpr uint64 Max = 18446744073709551615ULL;
};
template<> struct Limits<float> {
	static constexpr float Min = -3.402823466e+38F;
	static constexpr float Max = 3.402823466e+38F;
};
template<> struct Limits<double> {
	static constexpr double Min = -1.7976931348623158e+308;
	static constexpr double Max = 1.7976931348623158e+308;
};

};