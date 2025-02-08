// Copyright (c) 2024-2025, Hidde van der Kooij
// SPDX-License-Identifier: BSD-2-Clause

#include "Delegate.h"

DelegateEntry::DelegateEntry(const void* object, void (*function)(void*), void* context)
	: Object(object), Function(function), Context(context)
{}

void Delegate::Register(const void* object, void (*function)(void*), void* context)
{
	Entries.Add(DelegateEntry(object, function, context));
}

void Delegate::Unregister(const void *object)
{
	for (uint32 i = Entries.Num(); i > 0;)
	{
		--i;
		auto& entry = Entries[i];
		if (entry.Object == object)
			Entries.RemoveAtSwap(i);
	}
}

void Delegate::Invoke()
{
	for (auto& entry : Entries)
	{
		entry.Function(entry.Context);
	}
}