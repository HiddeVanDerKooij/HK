// Copyright (c) 2023-2024, Hidde van der Kooij
// SPDX-License-Identifier: BSD-2-Clause

#include <cmath>

#include "Common/Math.h"

f32 Math::Round(f32 a)
{
	return std::round(a);
}

f64 Math::Round(f64 a)
{
	return std::round(a);
}

f32 Math::Floor(f32 a)
{
	return std::floor(a);
}

f64 Math::Floor(f64 a)
{
	return std::floor(a);
}

f32 Math::Ceil(f32 a)
{
	return (f32)Math::Ceil((f64)a);
}

f64 Math::Ceil(f64 a)
{
	return std::ceil(a);
}