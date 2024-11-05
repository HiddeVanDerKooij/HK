// Copyright (c) 2023-2024, Hidde van der Kooij
// SPDX-License-Identifier: BSD-2-Clause

#pragma once

#include "Common/Types.h"
#include "Containers/Array.h"

class StringView
{
public:
	StringView();
	StringView(ENoInit);
	StringView(const StringView& other) = default;
	explicit StringView(const char8* string);
	StringView(const char8* string, uint32 size);

	StringView& operator=(const StringView& other) = default;

	const char8* operator*() const;
	bool operator==(const StringView& other) const;
	bool operator!=(const StringView& other) const;
	// Case insensitive comparison
	bool operator&&(const StringView& other) const;
	const char8* Data() const;
	uint32 Size() const;

	int32 LeftFind(char8 c, uint32 offset = 0) const;
	int32 RightFind(char8 c, uint32 offset = 0) const;
	bool StartsWith(const StringView& other) const;
	bool EndsWith(const StringView& other) const;
	char8 FirstChar() const;
	char8 LastChar() const;
	char8 At(uint32 index) const;
	char8 operator[](uint32 index) const;
	StringView Substring(uint32 offset, uint32 length) const;
	StringView SubstringSlow(uint32 offset, uint32 length) const;
	uint32 CountChar(char8 c) const;
	
	StringView ChopLeft(uint32 count) const;
	StringView ChopRight(uint32 count) const;
	
	Array<StringView> Split(char8 delimiter) const;
	
	bool ParseAsInt(int64& result, uint32 base) const;

private:
	const char8* StringData;
	uint32 StringSize;
};

const StringView operator""_sv(const char8* string, size64 size);