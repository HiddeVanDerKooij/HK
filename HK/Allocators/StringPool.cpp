// Copyright (c) 2023-2024, Hidde van der Kooij
// SPDX-License-Identifier: BSD-2-Clause

#include "Allocators/Memory.h"
#include "Allocators/StringPool.h"
#include "Common/Math.h"

// TODO (HvdK):
// When we allocate strings, pad them out till 8 bytes
// so our string compare is faster.

#define STRING_POOL_PAD 8

StringRef::StringRef(const StringView& InView, StringPool* InPool)
	: View(InView), Pool(InPool)
{
	Pool->AddRef(*this);
}

StringRef::~StringRef()
{
	Pool->ClearRef(*this);
}

StringPool::StringPool()
{
	Buffers.Reserve(16);
	BufferSizes.Reserve(16);
	FreeList.Reserve(16);
	Entries.Reserve(16);
}

StringPool::~StringPool()
{
	for (uint32 i = 0; i < Buffers.Num(); ++i)
	{
		Memory::Free(Buffers[i], BufferSizes[i]);
	}
}

StringView StringPool::InternPermanent(const char8* newentry, uint32 size)
{
	StringView view(newentry, size);
	return InternPermanent(view);
}

StringView StringPool::InternPermanent(const StringView& view)
{
	int32 entryIndex = FindEntry(view);

	if (entryIndex == -1) {
		return InsertPermanent(view);
	}

	entryIndex = FindEntry(view);
	StringEntry& entry = Entries[entryIndex];
	entry.RefCount = 0x7FFFFFFF;

	return StringView(Resolve(entry), view.Size());
}

StringRef StringPool::Intern(const char8* newentry, uint32 size)
{
	StringView view(newentry, size);
	return Intern(view);
}

StringRef StringPool::Intern(const StringView& view)
{
	int32 entryIndex = FindEntry(view);

	if (entryIndex == -1) {
		return Insert(view);
	}

	uint32 size = 0;
	const char8* data = Resolve(entryIndex, size);

	return StringRef(StringView(data, size), this);
}

void StringPool::AddRef(const StringRef& ref)
{
	int32 entryIndex = FindEntry(ref.View);
	CHECK(entryIndex != -1);
	auto& entry = Entries[entryIndex];
	++entry.RefCount;
}

void StringPool::ClearRef(const StringRef& ref)
{
	int32 entryIndex = FindEntry(ref.View);
	CHECK(entryIndex != -1);
	auto& entry = Entries[entryIndex];
	--entry.RefCount;
	if (entry.RefCount == 0)
	{
		// TODO (HvdK): Merge with existing freelist
		FreeListEntry freeEntry;
		freeEntry.BufferIndex = entry.BufferIndex;
		freeEntry.Position = entry.Position;
		freeEntry.Size = entry.Size;
		FreeList.Add(freeEntry);
		Entries.RemoveAtSwap(entryIndex);
	}
}

int32 StringPool::FindEntry(const StringView& view) const
{
	for (uint32 i = 0; i < Entries.Num(); ++i)
	{
		auto& entry = Entries[i];
		if (entry.Size != view.Size())
			continue;

		const char8* bytes = Resolve(entry);
		if (Memory::StringEquals(bytes, view.Data(), view.Size()))
		{
			return i;
		}
	}
	return -1;
}

const char8* StringPool::Resolve(uint32 entryIndex, uint32& size) const
{
	auto& entry = Entries[entryIndex];
	size = entry.Size;
	return Resolve(entry);
}

const char8* StringPool::Resolve(const StringEntry& entry) const
{
	return &Buffers[entry.BufferIndex][entry.Position];
}

char8* StringPool::ResolveMutable(const StringEntry& entry) const
{
	return &Buffers[entry.BufferIndex][entry.Position];
}

StringRef StringPool::Insert(const StringView& view)
{
	StringEntry entry = Allocate(view.Size());

	char8* entryData = ResolveMutable(entry);

	Memory::Copy(view.Data(), entryData, entry.Size);
	entryData[view.Size()] = '\0';

	return StringRef(StringView(entryData, entry.Size), this);
}

StringView StringPool::InsertPermanent(const StringView& view)
{
	StringEntry entry = Allocate(view.Size());

	entry.RefCount = 0x7FFFFFFF;
	char8* entryData = ResolveMutable(entry);

	Memory::Copy(view.Data(), entryData, entry.Size);
	entryData[view.Size()] = '\0';

	return StringView(entryData, view.Size());

}

StringPool::StringEntry& StringPool::Allocate(uint32 size)
{
	// TODO (HvdK): Smarter logic where we find a better fit
	// by scanning the free list more.

	uint32 FreeListIndex = INVALID_INDEX;

	for (uint32 i = FreeList.Num()-1; i < FreeList.Num(); --i) {
		auto& freeEntry = FreeList[i];
		if (freeEntry.Size < size)
			continue;

		// We found a free entry that is big enough.
		// We can use it.
		FreeListIndex = i;
		break;
	}

	if (FreeListIndex == INVALID_INDEX)
	{
		FreeListIndex = AllocateBuffer(size);
	}

	auto& freeEntry = FreeList[FreeListIndex];

	StringEntry entry;
	entry.RefCount = 0;
	entry.BufferIndex = freeEntry.BufferIndex;
	entry.Position = freeEntry.Position;
	entry.Size = size;

	auto& entryref = Entries.AddRef(entry);

	freeEntry.Size -= size;
	if (freeEntry.Size == 0)
	{
		FreeList.RemoveAtSwap(FreeListIndex);
	}
	else
	{
		freeEntry.Position += size;
	}

	return entryref;
}

int32 StringPool::AllocateBuffer(uint32 minsize)
{
	minsize = Math::Max(minsize, uint32(4096));

	Buffers.Add((char8*)Memory::Allocate(minsize));
	BufferSizes.Add(minsize);
	FreeList.Add(FreeListEntry{ Buffers.Num() - 1, 0, minsize });

	return FreeList.Num() - 1;
}