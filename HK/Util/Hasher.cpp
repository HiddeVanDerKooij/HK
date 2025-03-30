// Copyright (c) 2024, Hidde van der Kooij
// SPDX-License-Identifier: BSD-2-Clause

#include "Hasher.h"

#include "Containers/View/StringView.h"

Hasher::Hasher()
{
	State = 0xFF51AFD7ED558CCD;
}

void Hasher::HashItem(const uint8& item)
{
	Add(item);
}

void Hasher::HashItem(const uint16& item)
{
	Add(item);
}

void Hasher::HashItem(const uint32& item)
{
	Add(item);
}

void Hasher::HashItem(const uint64& item)
{
	Add(item);
}

void Hasher::HashItem(const StringView& view)
{
	const char8* data = view.Data();
	uint32 size = view.Size();
	
	while (size >= 8)
	{
		uint64 value = *reinterpret_cast<const uint64*>(data);
		Add(value);
		data += 8;
		size -= 8;
	}
	
	if (size >= 4)
	{
		uint32 value = *reinterpret_cast<const uint32*>(data);
		Add(value);
		data += 4;
		size -= 4;
	}
	
	if (size >= 2)
	{
		uint16 value = *reinterpret_cast<const uint16*>(data);
		Add(value);
		data += 2;
		size -= 2;
	}
	
	if (size >= 1)
	{
		uint8 value = *reinterpret_cast<const uint8*>(data);
		Add(value);
	}
}