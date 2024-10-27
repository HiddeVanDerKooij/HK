// Copyright (c) 2024, Hidde van der Kooij
// SPDX-License-Identifier: BSD-2-Clause

#pragma once

#include "Array.h"
#include "Util/Hasher.h"
#include "Common/CompilerMacros.h"
#include "Allocators/Memory.h"

template<typename T>
struct SetEntry {
	T Item;
	uint32 Hash;
	bool IsUsed;
};

constexpr f32 SetMaxLoadFactor = 0.75f;

template<typename T>
class Set {
public:
	typedef Hasher<uint32> HasherType;
	
	Set();
	Set(uint32 initialSize);
	Set(const Set& copy);
	Set(Set&& move);
	~Set();

	Set& operator=(const Set& other);
	Set& operator=(Set&& other);

	bool Contains(const T& item) const;
	bool Add(const T& item);
	uint32 AddGetIndex(const T& item);
	bool Remove(const T& item);
	void Clear();
	
	T* Find(const T& item);
	const T* Find(const T& item) const;
	T& FindOrAdd(const T& item);
	
	void GetItems(Array<T>& items) const;
	
protected:
	int32 FindIndex(const T& item) const;
	
private:
	bool AddImpl(const T& item, uint32* outIndex);

	void Rehash(uint32 newCapacity);
	void CheckGap(uint32 index);
	bool FixHole(uint32& index);

protected:
	SetEntry<T>* Entries;
	uint32 Capacity;
	uint32 NumEntries;
};

template<typename T>
Set<T>::Set()
{
	Entries = nullptr;
	Capacity = 0;
	NumEntries = 0;
}

template<typename T>
Set<T>::Set(uint32 buffer)
{
	if (buffer > 0) {
		Entries = nullptr;
		Capacity = 0;
		NumEntries = 0;
		Rehash(buffer);
	}
	else {
		Set();
	}
}

template<typename T>
Set<T>::Set(const Set& copy)
{
	Set();
	if (copy.NumEntries > 0) {
		Rehash(copy.Capacity);
		NumEntries = copy.NumEntries;
		for (uint32 i = 0; i < copy.Capacity; ++i) {
			if (copy.Entries[i].IsUsed) {
				Entries[i] = copy.Entries[i];
			}
		}
	}
}

template<typename T>
Set<T>::Set(Set&& move)
{
	Entries = move.Entries;
	Capacity = move.Capacity;
	NumEntries = move.NumEntries;

	move.Entries = nullptr;
	move.Capacity = 0;
	move.NumEntries = 0;
}

template<typename T>
Set<T>::~Set()
{
	Clear();
}

template<typename T>
Set<T>& Set<T>::operator=(const Set& other)
{
	CHECK(this != &other);
	Clear();
	if (other.NumEntries > 0) {
		Rehash(other.Capacity);
		NumEntries = other.NumEntries;
		for (uint32 i = 0; i < other.Capacity; ++i) {
			if (other.Entries[i].IsUsed) {
				Entries[i] = other.Entries[i];
			}
		}
	}
	return *this;
}

template<typename T>
Set<T>& Set<T>::operator=(Set&& other)
{
	CHECK(this != &other);
	Clear();
	Entries = other.Entries;
	Capacity = other.Capacity;
	NumEntries = other.NumEntries;

	other.Entries = nullptr;
	other.Capacity = 0;
	other.NumEntries = 0;
	return *this;
}

template<typename T>
bool Set<T>::Contains(const T& item) const
{
	return !!Find(item);
}

template<typename T>
bool Set<T>::Add(const T& item)
{
	return AddImpl(item, nullptr);
}

template<typename T>
uint32 Set<T>::AddGetIndex(const T& item)
{
	uint32 index = 0;
	AddImpl(item, &index);
	return index;
}

template<typename T>
bool Set<T>::Remove(const T& item)
{
	const uint32 hash = HasherType::Hash(item);
	uint32 index = hash % Capacity;
	const uint32 startIndex = index;
	do {
		if (!Entries[index].IsUsed) {
			return false;
		}
		if (Entries[index].Hash == hash && Entries[index].Item == item) {
			Entries[index].Item.~T();
			Entries[index].IsUsed = false;
			--NumEntries;
			CheckGap(index);
			return true;
		}
		++index;
		if (UNLIKELY(index == Capacity)) {
			index = 0;
		}
	} while (index != startIndex);
	return false;
}

template<typename T>
void Set<T>::Clear()
{
	if (LIKELY(Entries != nullptr)) {
		for (uint32 i = 0; i < Capacity; ++i) {
			if (Entries[i].IsUsed) {
				Entries[i].Item.~T();
			}
		}
		Memory::Free(Entries, sizeof(SetEntry<T>)*Capacity);
	}
	Entries = nullptr;
	Capacity = 0;
	NumEntries = 0;
}


template<typename T>
T* Set<T>::Find(const T& item)
{
	const int32 index = FindIndex(item);
	if (index >= 0) {
		return &Entries[index].Item;
	}
	return nullptr;
}

template<typename T>
const T* Set<T>::Find(const T& item) const
{
	const int32 index = FindIndex(item);
	if (index >= 0) {
		return &Entries[index].Item;
	}
	return nullptr;
}

template<typename T>
T& Set<T>::FindOrAdd(const T& item)
{
	T* found = Find(item);
	if (!!found) {
		return *found;
	}
	return Entries[AddGetIndex(item)].Item;
}


template<typename T>
void Set<T>::GetItems(Array<T>& items) const
{
	items.Reset();
	items.Reserve(NumEntries);
	
	SetEntry<T>* entry = Entries;
	uint32 count = NumEntries;
	while (count > 0) {
		--count;
		while (!entry->IsUsed) {
			++entry;
		}
		items.Add(entry->Item);
		++entry;
	}
}

template<typename T>
int32 Set<T>::FindIndex(const T& item) const
{
	if (UNLIKELY(Entries == nullptr)) {
		return -1;
	}
	const uint32 hash = HasherType::Hash(item);
	uint32 index = hash % Capacity;
	const uint32 startIndex = index;
	do {
		if (!Entries[index].IsUsed) {
			return -1;
		}
		if (Entries[index].Hash == hash && Entries[index].Item == item) {
			return index;
		}
		++index;
		if (UNLIKELY(index == Capacity)) {
			index = 0;
		}
	} while (index != startIndex);
	return -1;
}

template<typename T>
bool Set<T>::AddImpl(const T& item, uint32* outIndex)
{
	if (UNLIKELY(NumEntries >= SetMaxLoadFactor * Capacity)) {
		Rehash(Capacity * 2);
	}
	const uint32 hash = HasherType::Hash(item);
	uint32 index = hash % Capacity;
	const uint32 startIndex = index;
	do {
		if (!Entries[index].IsUsed) {
			Entries[index].Item = item;
			Entries[index].Hash = hash;
			Entries[index].IsUsed = true;
			++NumEntries;
			if (outIndex)
			{
				*outIndex = index;
			}
			return true;
		}
		if (Entries[index].Hash == hash && Entries[index].Item == item) {
			if (outIndex)
			{
				*outIndex = index;
			}
			return false;
		}
		++index;
		if (UNLIKELY(index == Capacity)) {
			index = 0;
		}
	} while (index != startIndex);
	CHECK(false);
	return false;
}

template<typename T>
void Set<T>::Rehash(uint32 newCapacity)
{
	if (UNLIKELY(newCapacity < 8)) {
		newCapacity = 8;
	}
	if (UNLIKELY(newCapacity < NumEntries)) {
		newCapacity = NumEntries;
	}
	if (UNLIKELY(newCapacity == Capacity)) {
		return;
	}
	
	SetEntry<T>* oldEntries = Entries;
	uint32 oldCapacity = Capacity;
	uint32 oldNumEntries = NumEntries;
	
	uint64 bytesForEntries = sizeof(SetEntry<T>)*newCapacity;
	Entries = static_cast<SetEntry<T>*>(Memory::Allocate(bytesForEntries));
	Memory::FillZero(Entries, bytesForEntries);
	Capacity = newCapacity;
	NumEntries = 0;
	
	if (LIKELY(oldEntries != nullptr)) {
		SetEntry<T>* oldEntry = &oldEntries[0];
		uint32 count = oldNumEntries;
		while (count > 0) {
			--count;
			while (!oldEntry->IsUsed) {
				++oldEntry;
			}
			Add(oldEntry->Item);
			++oldEntry;
		}

		Memory::Free(oldEntries, sizeof(SetEntry<T>)*oldCapacity);
	}
}

template<typename T>
void Set<T>::CheckGap(uint32 index)
{
	while (FixHole(index)) {}
}

template<typename T>
bool Set<T>::FixHole(uint32& index)
{
	CHECK(index >= 0 && index < Capacity);
	CHECK(!Entries[index].IsUsed);
	
	// Look from index onwards if any entries can be moved to the gap
	// The strategy is to look for the next gap and then find the last
	// entry that would fit in this gap, and move it there.
	// Then we repeat this for the gap left by the item we moved.
	
	uint32 start = index;
	uint32 end = start;
	
	// Scan for last gap
	while (Entries[(end+1) % Capacity].IsUsed) {
		++end;
	}
	
	// Scan for first fit
	while (end != start) {
		const uint32 entryIndex = end % Capacity;
		uint32 idealPosition = Entries[entryIndex].Hash;
		uint32 offsetCurrent = (end - idealPosition) % Capacity;
		uint32 offsetGap = (index - idealPosition) % Capacity;
		if (offsetGap < offsetCurrent)
		{
			Entries[index] = Move(Entries[entryIndex]);
			Entries[entryIndex].IsUsed = false;
			// TODO (HvdK): call destructor?
			index = entryIndex;
			return true;
		}
		--end;
	}
	return false;
}