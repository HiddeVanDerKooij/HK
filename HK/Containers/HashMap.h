#pragma once

#include "Common/Types.h"

template<class Key, class Value>
class HashMap {
public:
	HashMap();
	HashMap(uint32 capacity);
	HashMap(const HashMap& copy);
	HashMap(HashMap&& move);
	~HashMap();

	HashMap& operator=(const HashMap& other);
	HashMap& operator=(HashMap&& other);

	Value& operator[](const Key& key);
	const Value& operator[](const Key& key) const;

	bool Contains(const Key& key) const;
	bool Remove(const Key& key);

	uint32 Num() const;
	uint32 Capacity() const;

	void Clear();
};