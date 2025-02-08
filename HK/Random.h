// Copyright (c) 2024-2025, Hidde van der Kooij
// SPDX-License-Identifier: BSD-2-Clause

#pragma once

#include "Common/Types.h"

namespace Random
{
	struct RandState
	{
		uint64 State[2];
		
		void Seed(uint64 seed);
		void Advance();
		
		bool RandBool();
		uint8 RandU8();
		uint16 RandU16();
		uint32 RandU32();
		uint64 RandU64();
		f32 RandF32();
		f64 RandF64();
	};
	
	bool RandBool(RandState* state = nullptr);
	uint8 RandU8(RandState* state = nullptr);
	uint16 RandU16(RandState* state = nullptr);
	uint32 RandU32(RandState* state = nullptr);
	uint64 RandU64(RandState* state = nullptr);
	f32 RandF32(RandState* state = nullptr);
	f64 RandF64(RandState* state = nullptr);
	
	RandState* GetGlobalState();
}