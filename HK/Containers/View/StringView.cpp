// Copyright (c) 2023-2024, Hidde van der Kooij
// SPDX-License-Identifier: BSD-2-Clause

#include "Allocators/Memory.h"
#include "Common/CompilerMacros.h"
#include "Containers/View/StringView.h"

static const char8* empty = "";

StringView::StringView()
	: StringData(empty)
	, StringSize(0)
{
}

StringView::StringView(ENoInit) {}

StringView::StringView(const char8* string)
	: StringData(string)
	, StringSize(0)
{
	CHECK(string != nullptr);
	StringSize = Memory::StringSize(string);
	CHECK(StringSize >= 0);
}

StringView::StringView(const char8* string, uint32 size)
	: StringData(string)
	, StringSize(size)
{
	CHECK(string != nullptr || size == 0);
	CHECK(size >= 0);
}

const char8* StringView::operator*() const
{
	return StringData;
}

bool StringView::operator==(const StringView& other) const
{
	return Memory::StringEquals(*this, other);
}

bool StringView::operator!=(const StringView& other) const
{
	return !Memory::StringEquals(*this, other);
}

bool StringView::operator&&(const StringView& other) const
{
	if (Size() != other.Size())
		return false;
	// TODO (HvdK): Optimize this
	for (uint32 i = 0; i < Size(); ++i) {
		char8 a = Data()[i];
		char8 b = other.Data()[i];
		if (a >= 'A' && a <= 'Z') {
			a += 'a' - 'A';
		}
		if (b >= 'A' && b <= 'Z') {
			b += 'a' - 'A';
		}
		if (a != b) {
			return false;
		}
	}
	return true;
}

const char8* StringView::Data() const
{
	return StringData;
}

uint32 StringView::Size() const
{
	return StringSize;
}

int32 StringView::LeftFind(char8 c, uint32 offset) const
{
	for (uint32 i=offset; i<StringSize; ++i) {
		if (StringData[i] == c) return i;
	}
	return -1;
}

int32 StringView::RightFind(char8 c, uint32 offset) const
{
	for (uint32 i=StringSize - offset - 1; i<StringSize; --i) {
		if (StringData[i] == c) return i;
	}
	return -1;
}

bool StringView::StartsWith(const StringView& other) const
{
	if (other.Size() > Size()) return false;
	for (uint32 i=0; i<other.Size(); ++i) {
		if (StringData[i] != other.Data()[i]) return false;
	}
	return true;
}

bool StringView::EndsWith(const StringView& other) const
{
	if (other.Size() > Size()) return false;
	for (uint32 i=0; i<other.Size(); ++i) {
		if (StringData[StringSize - other.Size() + i] != other.Data()[i]) return false;
	}
	return true;
}

char8 StringView::FirstChar() const
{
	if (UNLIKELY(StringSize == 0)) return 0;
	return StringData[0];
}

char8 StringView::LastChar() const
{
	if (UNLIKELY(StringSize == 0)) return 0;
	return StringData[StringSize - 1];
}

char8 StringView::At(uint32 index) const
{
	if (UNLIKELY(index >= StringSize)) return 0;
	return StringData[index];
}

char8 StringView::operator[](uint32 index) const
{
	return At(index);
}

StringView StringView::Substring(uint32 offset, uint32 length) const
{
	CHECK(offset <= StringSize);
	CHECK(offset + length <= StringSize);

	return StringView(&StringData[offset], length);
}

StringView StringView::SubstringSlow(uint32 offset, uint32 length) const
{
	if (offset >= StringSize)
	{
		offset = 0;
	}
	if (offset + length > StringSize)
	{
		length = StringSize - offset;
	}
	return Substring(offset, length);
}

uint32 StringView::CountChar(char8 c) const
{
	uint32 count = 0;
	for (uint32 i=0; i<StringSize; ++i) {
		if (StringData[i] == c) ++count;
	}
	return count;
}

StringView StringView::ChopLeft(uint32 num) const
{
	CHECK(num <= StringSize);
	return StringView(&StringData[num], StringSize - num);
}

StringView StringView::ChopRight(uint32 num) const
{
	CHECK(num <= StringSize);
	return StringView(StringData, StringSize - num);
}

Array<StringView> StringView::Split(char8 delimiter) const
{
	Array<StringView> result;
	uint32 start = 0;
	for (uint32 i=0; i<StringSize; ++i) {
		if (StringData[i] == delimiter) {
			result.Add(Substring(start, i - start));
			start = i + 1;
		}
	}
	if (start < StringSize) {
		result.Add(Substring(start, StringSize - start));
	}
	return result;
}

bool StringView::ParseAsInt(int64& result, uint32 base) const
{
	CHECK(base >= 2 && base <= 36);
	result = 0;
	uint32 i = 0;
	bool negative = false;
	if (StringData[i] == '-') {
		negative = true;
		++i;
	}
	for (; i < StringSize; ++i) {
		char8 c = StringData[i];
		int64 digit = 0;
		if (c >= '0' && c <= '9') {
			digit = c - '0';
		} else if (c >= 'a' && c <= 'z') {
			digit = c - 'a' + 10;
		} else if (c >= 'A' && c <= 'Z') {
			digit = c - 'A' + 10;
		} else {
			return false;
		}
		if (digit >= base) {
			return false;
		}
		result = result * base + digit;
	}
	if (negative) {
		result = -result;
	}
	return true;
}

const StringView operator""_sv(const char8* string, size64 size)
{
	return StringView(string, static_cast<uint32>(size));
};