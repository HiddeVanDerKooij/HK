// Copyright (c) 2023-2024, Hidde van der Kooij
// SPDX-License-Identifier: BSD-2-Clause

#pragma once

#include "Containers/Array.h"
#include "Common/Types.h"
#include "Containers/View/StringView.h"

class StringPool;

// A reference to a string in the pool. This is a view into the string
// and does not own the memory. It must be cleared before the pool is
// destroyed.
struct StringRef {
	StringRef(const StringView& InView, class StringPool* InPool);
	~StringRef();

	StringView View;
private:
	StringPool* Pool;
};

// A pool of strings. Strings are interned into the pool and are
// guaranteed to be unique. A string can be resolved into a StringRef
// which is a view into the string.
class StringPool {
private:
	struct StringEntry {
		uint32 RefCount;
		uint32 BufferIndex;
		uint32 Position;
		uint32 Size;
	};
	struct FreeListEntry {
		uint32 BufferIndex;
		uint32 Position;
		uint32 Size;
	};

public:
	StringPool();
	~StringPool();

	StringView InternPermanent(const char8* newentry, uint32 size);
	StringView InternPermanent(const StringView& view);
	StringRef Intern(const char8* newentry, uint32 size);
	StringRef Intern(const StringView& view);

	void AddRef(const StringRef& ref);
	void ClearRef(const StringRef& ref);

private:
	// TODO (HvdK): Don't use int32, use uint32
	int32 FindEntry(const StringView& view) const;
	const char8* Resolve(uint32 entryIndex, uint32& size) const;
	const char8* Resolve(const StringEntry& entry) const;
	char8* ResolveMutable(const StringEntry& entry) const;
	StringRef Insert(const StringView& view);
	StringView InsertPermanent(const StringView& view);

	StringEntry& Allocate(uint32 size);
	// Returns the freelist index associated with this allocation
	int32 AllocateBuffer(uint32 minsize);

	Array<char8*> Buffers;
	Array<uint32> BufferSizes;

	// TODO (HvdK): Separate FreeList class
	Array<FreeListEntry> FreeList;
	Array<StringEntry> Entries;
};
