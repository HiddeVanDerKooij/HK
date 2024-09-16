#pragma once

#include "Common/Types.h"

class StringView
{
public:
	StringView();
	StringView(ENoInit);
	StringView(const StringView& other) = default;
	StringView(const char8* string);
	StringView(const char8* string, uint32 size);

	StringView& operator=(const StringView& other) = default;

	const char8* operator*() const;
	bool operator==(const StringView& other) const;
	bool operator!=(const StringView& other) const;
	// Case insensitive comparison
	bool operator&&(const StringView& other) const;
	const char8* Data() const;
	uint32 Size() const;

	int32 LeftFind(char8 c, uint32 offset) const;
	int32 RightFind(char8 c, uint32 offset) const;
	StringView Substring(uint32 offset, uint32 length) const;
	uint32 CountChar(char8 c) const;
	
	StringView ChopLeft(uint32 count) const;
	StringView ChopRight(uint32 count) const;
	
	bool ParseAsInt(int64& result, uint32 base) const;

private:
	const char8* StringData;
	uint32 StringSize;
};

const StringView operator""_sv(const char8* string, size64 size);