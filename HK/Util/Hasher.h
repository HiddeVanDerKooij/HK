// Copyright (c) 2024, Hidde van der Kooij
// SPDX-License-Identifier: BSD-2-Clause

#pragma once

#include "Common/Types.h"

template<typename V>
class Hasher {
public:
	Hasher();
	
	template<typename U>
	void Add(const U& v);
	
	V Get() const;
	
	template<typename T>
	static V Hash(const T& item);
	
	template<typename T>
	void HashItem(const T& item);
	void HashItem(const uint32& item);

protected:
	uint64 State;
};

template<typename V>
Hasher<V>::Hasher()
{
	State = 0xFF51AFD7ED558CCD;
}

template<typename V>
template<typename U>
void Hasher<V>::Add(const U& v)
{
	const uint8* p = reinterpret_cast<const uint8*>(&v);
	uint32 size = sizeof(U);
	while (size >= 4) {
		uint32 value = *reinterpret_cast<const uint32*>(p);
		State ^= value;
		State *= 0x1000193;
		p += 4;
		size -= 4;
	}
	if (size >= 2) {
		uint16 value = *reinterpret_cast<const uint16*>(p);
		State ^= value;
		State *= 0x1000193;
		p += 2;
		size -= 2;
	}
	if (size == 1) {
		uint8 value = *reinterpret_cast<const uint8*>(p);
		State ^= value;
		State *= 0x1000193;
	}
}

template<typename V>
V Hasher<V>::Get() const
{
	static_assert(sizeof(State) >= sizeof(V));
	uint32 multiple = sizeof(State) / sizeof(V);
	uint64 state = State;
	V output = V(0xC4CEB9FE1A85EC53);
	while (multiple > 0) {
		output ^= V(state);
		state >>= sizeof(V) * 8;
		--multiple;
	}
	return output;
}

template<typename V>
template<typename T>
V Hasher<V>::Hash(const T& item)
{
	Hasher<V> hasher;
	hasher.HashItem(item);
	return hasher.Get();
}

template<typename V>
template<typename T>
void Hasher<V>::HashItem(const T& item)
{
	item.Hash(*this);
}

template<typename V>
void Hasher<V>::HashItem(const uint32& item)
{
	Add(item);
}