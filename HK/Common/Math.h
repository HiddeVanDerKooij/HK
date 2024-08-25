#pragma once

#include "Common/Types.h"

namespace Math {
	template<typename T, typename O>
	T Min(T a, O b) {
		return a < b ? a : b;
	};
	template<typename T, typename O>
	T Max(T a, O b) {
		return a > b ? a : b;
	};
};