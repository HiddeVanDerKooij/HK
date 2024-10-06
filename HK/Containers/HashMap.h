// Copyright (c) 2024, Hidde van der Kooij
// SPDX-License-Identifier: BSD-2-Clause

#pragma once

#include "Set.h"

template<class TKey, class TValue>
struct HashMapEntry {
	TKey Key;
	TValue Value;
	
	HashMapEntry() {}
	HashMapEntry(const TKey& key)
		: Key(key)
		, Value(TValue())
	{}
	
	void Hash(Hasher<uint32>& hasher) const {
		hasher.HashItem(Key);
	}
	
	bool operator==(const HashMapEntry& other) const {
		return Key == other.Key;
	}
};

template<class TKey, class TValue>
class HashMap : protected Set<HashMapEntry<TKey, TValue>> {
public:
	typedef Set<HashMapEntry<TKey, TValue>> Super;
	HashMap();
	HashMap(uint32 initialSize);
	HashMap(const HashMap& copy);
	HashMap(HashMap&& move);
	~HashMap();

	HashMap& operator=(const HashMap& other);
	HashMap& operator=(HashMap&& other);

	TValue& Add(const TKey& key);
	TValue& FindOrAdd(const TKey& key);
	TValue* Find(const TKey& key);
	const TValue* Find(const TKey& key) const;
	TValue& FindChecked(const TKey& key);
	const TValue& FindChecked(const TKey& key) const;

	void Remove(const TKey& key);
	void Clear();
	
	void GetItems(Array<HashMapEntry<TKey, TValue>>& items) const;
};

template<class TKey, class TValue>
HashMap<TKey, TValue>::HashMap()
	: Super()
{}

template<class TKey, class TValue>
HashMap<TKey, TValue>::HashMap(uint32 initialSize)
	: Super(initialSize)
{}

template<class TKey, class TValue>
HashMap<TKey, TValue>::HashMap(const HashMap& copy)
	: Super(copy)
{}

template<class TKey, class TValue>
HashMap<TKey, TValue>::HashMap(HashMap&& move)
	: Super(move)
{}

template<class TKey, class TValue>
HashMap<TKey, TValue>::~HashMap()
{}

template<class TKey, class TValue>
HashMap<TKey, TValue>& HashMap<TKey, TValue>::operator=(const HashMap& other)
{
	Super::operator=(other);
	return *this;
}

template<class TKey, class TValue>
HashMap<TKey, TValue>& HashMap<TKey, TValue>::operator=(HashMap&& other)
{
	Super::operator=(other);
	return *this;
}

template<class TKey, class TValue>
TValue& HashMap<TKey, TValue>::Add(const TKey& key)
{
	HashMapEntry<TKey, TValue> entry;
	entry.Key = key;
	entry.Value = TValue();
	Super::Add(entry);
	uint32 index = Super::Find(HashMapEntry<TKey, TValue>(key));
	return Super::Entries[index].Item.Value;
}

template<class TKey, class TValue>
TValue& HashMap<TKey, TValue>::FindOrAdd(const TKey& key)
{
	uint32 index = Super::Find(HashMapEntry<TKey, TValue>(key), &index);
	if (index != -1) {
		return Super::Entries[index].Item.Value;
	}
	
	HashMapEntry<TKey, TValue> entry;
	entry.Key = key;
	entry.Value = TValue();
	Super::Add(entry);
	return Super::Entries[Super::Find(HashMapEntry<TKey, TValue>(key), &index)].Value;
}

template<class TKey, class TValue>
TValue* HashMap<TKey, TValue>::Find(const TKey& key)
{
	uint32 index = Super::Find(HashMapEntry<TKey, TValue>(key));
	if (index != -1) {
		return &Super::Entries[index].Item.Value;
	}
	return nullptr;
}

template<class TKey, class TValue>
const TValue* HashMap<TKey, TValue>::Find(const TKey& key) const
{
	int32 index = Super::Find(HashMapEntry<TKey, TValue>(key));
	if (index != -1) {
		return &Super::Entries[index].Item.Value;
	}
	return nullptr;
}

template<class TKey, class TValue>
TValue& HashMap<TKey, TValue>::FindChecked(const TKey& key)
{
	int32 index = Super::Find(HashMapEntry<TKey, TValue>(key));
	CHECK(index != -1);
	return Super::Entries[index].Item.Value;
}

template<class TKey, class TValue>
const TValue& HashMap<TKey, TValue>::FindChecked(const TKey& key) const
{
	int32 index = Super::Find(HashMapEntry<TKey, TValue>(key));
	CHECK(index != -1);
	return Super::Entries[index].Item.Value;
}

template<class TKey, class TValue>
void HashMap<TKey, TValue>::Remove(const TKey& key)
{
	Super::Remove(HashMapEntry<TKey, TValue>(key));
}

template<class TKey, class TValue>
void HashMap<TKey, TValue>::Clear()
{
	Super::Clear();
}

template<class TKey, class TValue>
void HashMap<TKey, TValue>::GetItems(Array<HashMapEntry<TKey, TValue>>& items) const
{
	Super::GetItems(items);
}