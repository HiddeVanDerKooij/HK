// Copyright (c) 2024-2025, Hidde van der Kooij
// SPDX-License-Identifier: BSD-2-Clause

#include "Random.h"

using namespace Random;

static RandState GRandState;

void RandState::Seed(uint64 seed)
{
	State[0] = seed;
	State[1] = 0x6C8E9CF570932BD5;
};

void RandState::Advance()
{
	State[0] ^= 0x9E3779B97F4A7C15;
	State[0] *= 0x3D4D2B4B1C3A9D5C;
	State[1] ^= 0x1D872B413D8C9D5C;
	State[1] *= 0x6C8E9CF570932BD5;
}

template <typename T>
T RandUInt(RandState *state)
{
	if (state == nullptr)
	{
		state = &GRandState;
	}
	state->Advance();

	uint64 r = state->State[0];
	r ^= (state->State[1] >> 32) | (state->State[1] << 32);

	return T(r);
}

bool RandState::RandBool()
{
	return RandUInt<uint8>(this) & 1;
}

uint8 RandState::RandU8()
{
	return RandUInt<uint8>(this);
}

uint16 RandState::RandU16()
{
	return RandUInt<uint16>(this);
}

uint32 RandState::RandU32()
{
	return RandUInt<uint32>(this);
}

uint64 RandState::RandU64()
{
	return RandUInt<uint64>(this);
}

f32 RandState::RandF32()
{
	return RandUInt<uint32>(this) / f32(uint32(-1));
}

f64 RandState::RandF64()
{
	return RandUInt<uint64>(this) / f64(uint64(-1));
}

//

bool Random::RandBool(RandState *state)
{
	if (!state)
		state = GetGlobalState();
	return state->RandBool();
}

uint8 Random::RandU8(RandState *state)
{
	if (!state)
		state = GetGlobalState();
	return state->RandU8();
}

uint16 Random::RandU16(RandState *state)
{
	if (!state)
		state = GetGlobalState();
	return state->RandU16();
}

uint32 Random::RandU32(RandState *state)
{
	if (!state)
		state = GetGlobalState();
	return state->RandU32();
}

uint64 Random::RandU64(RandState *state)
{
	if (!state)
		state = GetGlobalState();
	return state->RandU64();
}

f32 Random::RandF32(RandState *state)
{
	if (!state)
		state = GetGlobalState();
	return state->RandF32();
}

f64 Random::RandF64(RandState *state)
{
	if (!state)
		state = GetGlobalState();
	return state->RandF64();
}

RandState *Random::GetGlobalState()
{
	return &GRandState;
}