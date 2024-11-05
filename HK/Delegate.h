#pragma once

#include "Common/Types.h"
#include "Containers/Array.h"

struct DelegateEntry
{
	const void* Object;
	void (*Function)(void*);
	void* Context;
	
	DelegateEntry() = default;
	DelegateEntry(const void* object, void (*function)(void*), void* context);
};

class Delegate
{
public:
	void Register(const void* object, void (*function)(void*), void* context);
	void Unregister(const void* object);
	void Invoke();
	
protected:
	Array<DelegateEntry> Entries;
};