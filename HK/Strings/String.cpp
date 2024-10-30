// Copyright (c) 2023-2024, Hidde van der Kooij
// SPDX-License-Identifier: BSD-2-Clause

#include "Strings/String.h"

#include "Allocators/Memory.h"
#include "Common/CompilerMacros.h"

uint8 CurrentBuffer = 0;
char8 Buffer[16][256] = { '\0' };

uint8 GetCurrentBufferIndexAndIncrement() {
	uint8 result = CurrentBuffer;
	CurrentBuffer = (CurrentBuffer + 1) % 16;
	return result;
}

Format::__HexUint8 Format::AsHex8(uint8 value) {
	return *reinterpret_cast<__HexUint8*>(&value);
}

Format::__HexUint16 Format::AsHex16(uint16 value) {
	return *reinterpret_cast<__HexUint16*>(&value);
}

Format::__HexUint24 Format::AsHex24(uint32 value) {
	return *reinterpret_cast<__HexUint24*>(&value);
}

Format::__HexUint32 Format::AsHex32(uint32 value) {
	return *reinterpret_cast<__HexUint32*>(&value);
}

Format::__HexUint64 Format::AsHex64(uint64 value) {
	return *reinterpret_cast<__HexUint64*>(&value);
}

Format::__HexUint64 Format::AsHex64(const void* value) {
	return *reinterpret_cast<__HexUint64*>(&value);
}

AnsiString::AnsiString() : Array<char8>() {}

AnsiString::AnsiString(const AnsiString& copy) : Array<char8>(copy.ArrayMax) {
	if (LIKELY(copy.ArrayNum > 0)) {
		ArrayNum = copy.ArrayNum;
		Memory::Copy(copy.Data, Data, ElementSize * (ArrayNum));
	}
}

AnsiString::AnsiString(AnsiString&& move) : Array<char8>(Move(move)) {}

AnsiString::AnsiString(char8 c) : Array<char8>(1) {
	Data[0] = c;
	ArrayNum = 1;
}

AnsiString::AnsiString(uint32 buffer) : Array<char8>(buffer) {}

AnsiString::AnsiString(StringView view) : Array<char8>(view.Size()) {
	if (LIKELY(view.Size() > 0)) {
		Memory::Copy(view.Data(), Data, view.Size());
	}
	ArrayNum = view.Size();
}

char8 AnsiString::At(uint32 index) const {
	ASSERT(index < ArrayNum);
	return Data[index];
}

AnsiString& AnsiString::operator=(const AnsiString& other) {
	if (LIKELY(this != &other)) {
		RequireArrayMaxGrowth(other.ArrayNum+1);
		Memory::Copy(other.Data, Data, ElementSize * (other.ArrayNum));
		ArrayNum = other.ArrayNum;
		Data[ArrayNum] = '\0';
	}
	return *this;
}

AnsiString& AnsiString::operator=(AnsiString&& other) {
	CHECK(this != &other);
	Array<char8>::operator=(Move(other));
	return *this;
}

AnsiString& AnsiString::operator=(StringView view) {
	RequireArrayMaxGrowth(view.Size());
	Memory::Copy(view.Data(), Data, view.Size());
	ArrayNum = view.Size();
	return *this;
}

AnsiString& AnsiString::operator+=(const AnsiString& other) {
	Append(other.AsView());
	return *this;
}

AnsiString& AnsiString::operator+=(StringView view) {
	Append(view);
	return *this;
}

AnsiString& AnsiString::operator+=(const char8* other) {
	Append(StringView(other));
	return *this;
}

AnsiString& AnsiString::operator+=(char8 c) {
	RequireArrayMaxGrowth(ArrayNum + 1);
	Data[ArrayNum++] = c;
	return *this;
}

AnsiString AnsiString::operator+(const AnsiString& other) {
	AnsiString result(ArrayNum + other.ArrayNum);
	result.Append(AsView());
	result.Append(other.AsView());
	return Move(result);
}

AnsiString AnsiString::operator+(StringView view) {
	AnsiString result(ArrayNum + view.Size());
	result.Append(AsView());
	result.Append(view);
	return Move(result);
}

AnsiString AnsiString::operator+(const char8* other) {
	AnsiString result(ArrayNum + StringView(other).Size());
	result.Append(AsView());
	result.Append(StringView(other));
	return Move(result);
}

const char8* AnsiString::operator*() {
	return AsCString();
}

const char8* AnsiString::operator*() const {
	return AsCString();
}

bool AnsiString::operator==(const AnsiString& other) const
{
	return AsView() == other.AsView();
}
bool AnsiString::operator!=(const AnsiString& other) const
{
	return AsView() != other.AsView();
}
bool AnsiString::operator==(const StringView& other) const
{
	return AsView() == other;
}
bool AnsiString::operator!=(const StringView& other) const
{
	return AsView() != other;
}

void AnsiString::RemoveLast(uint32 n)
{
	ASSERT(n <= ArrayNum);
	ArrayNum -= n;
	Data[ArrayNum] = '\0';
}

uint32 AnsiString::Size() const {
	return ArrayNum;
}

void AnsiString::Append(StringView view) {
	RequireArrayMaxGrowth(ArrayNum + view.Size());
	Memory::Copy(view.Data(), Data + ArrayNum, view.Size());
	ArrayNum += view.Size();
}

StringView AnsiString::AsView() const {
	return StringView(Data, ArrayNum);
}

const char8* AnsiString::AsCString() {
	EnsureNullTerminated();
	return Data;
}

const char8* AnsiString::AsCString() const {
	CheckNullTerminated();
	return Data;
}

void AnsiString::Trim() {
	while (ArrayNum > 0 && Data[ArrayNum - 1] == ' ') {
		--ArrayNum;
	}
}

AnsiString AnsiString::Repeated(const char8 c, uint32 count) {
	AnsiString result(count);
	result.ArrayNum = count;
	for (uint32 i = 0; i < count; ++i) {
		result[i] = c;
	}
	return Move(result);
}

bool AnsiString::ParseAsInt(int64& result, uint32 base) const {
	return AsView().ParseAsInt(result, base);
}

void AnsiString::EnsureNullTerminated() {
	if ((ArrayNum < ArrayMax) && (Data[ArrayNum] == '\0'))
		return;
	RequireArrayMaxGrowth(ArrayNum+1);
	Data[ArrayNum] = '\0';
}

void AnsiString::CheckNullTerminated() const {
	if (UNLIKELY(Data != nullptr))
		ASSERT((ArrayNum < ArrayMax) && (Data[ArrayNum] == '\0'));
}

StringView AnsiString::ConvertParam(const AnsiString& v) {
	return v.AsView();
}

StringView AnsiString::ConvertParam(StringView v) {
	return v;
}

StringView AnsiString::ConvertParam(const char8* v) {
	return StringView(v);
}

StringView AnsiString::ConvertParam(bool v) {
	return v ? "true"_sv : "false"_sv;
}

StringView AnsiString::ConvertParam(const void* v) {
	return ConvertParam(Format::AsHex64(v));
}

static StringView ConvertInteger(uint64 v, char8* b) {
	char* t = b;
	*t = '\0';
	do {
		*++t = '0' + (v % 10);
		v /= 10;
	} while (v != 0);

	uint32 l = uint32(t - Buffer[CurrentBuffer]);

	while (b < t) {
		char8 c = *b;
		*b++ = *t;
		*t-- = c;
	}
	return StringView(Buffer[GetCurrentBufferIndexAndIncrement()], l);
}

static StringView ConvertSignedInteger(int64 v) {
	if (v < 0) {
		Buffer[CurrentBuffer][0] = '-';
		return ConvertInteger(-v, Buffer[CurrentBuffer] + 1);
	} else {
		return ConvertInteger(v, Buffer[CurrentBuffer]);
	}
}

static StringView ConvertUnsignedInteger(uint64 v) {
	return ConvertInteger(v, Buffer[CurrentBuffer]);
}

StringView AnsiString::ConvertParam(int8 v) {
	return ConvertSignedInteger(v);
}

StringView AnsiString::ConvertParam(uint8 v) {
	return ConvertUnsignedInteger(v);
}

StringView AnsiString::ConvertParam(int16 v) {
	return ConvertSignedInteger(v);
}

StringView AnsiString::ConvertParam(uint16 v) {
	return ConvertUnsignedInteger(v);
}

StringView AnsiString::ConvertParam(int32 v) {
	return ConvertSignedInteger(v);
}

StringView AnsiString::ConvertParam(uint32 v) {
	return ConvertUnsignedInteger(v);
}

StringView AnsiString::ConvertParam(int64 v) {
	return ConvertSignedInteger(v);
}

StringView AnsiString::ConvertParam(uint64 v) {
	return ConvertUnsignedInteger(v);
}

StringView AnsiString::ConvertParam(f32 v)
{
	return ConvertParam(f64(v));
}

StringView AnsiString::ConvertParam(f64 v)
{
	char8* t = Buffer[CurrentBuffer];
	const char8* start = t;
	if (v < 0.)
	{
		*t++ = '-';
		v = -v;
	}
	uint64 n = uint64(v);
	
	{
		char8* s = t;
		do {
			*++t = '0' + (n % 10);
			n /= 10;
		} while (n != 0);

		char8* end = t;

		while (s < t) {
			char8 c = *s;
			*s++ = *t;
			*t-- = c;
		}
		t = end;
	}
	*t++ = '.';
	
	v -= uint64(v);
	for (uint32 i=0; i<4; ++i)
	{
		v *= 10.;
		uint32 d = uint32(v);
		v -= d;
		*t++ = '0'+d;
	}
	return StringView(Buffer[GetCurrentBufferIndexAndIncrement()], t - start);
}

void ConvertOneHexDigit(uint8 v, char8*& t) {
	uint8 m = 0b1111;
	const char8* hex = "0123456789ABCDEF";
	*t++ = hex[(v >> 4) & m];
	*t++ = hex[v & m];
}

StringView AnsiString::ConvertParam(Format::__HexUint8 v) {
	char8* t = Buffer[CurrentBuffer];
	*t++ = '0';
	*t++ = 'x';
	
	ConvertOneHexDigit(v.value, t);
	return StringView(Buffer[GetCurrentBufferIndexAndIncrement()], 4);
}

StringView AnsiString::ConvertParam(Format::__HexUint16 v) {
	char8* t = Buffer[CurrentBuffer];
	*t++ = '0';
	*t++ = 'x';
	
	ConvertOneHexDigit((v.value >> 8) & 0xFF, t);
	ConvertOneHexDigit(v.value & 0xFF, t);
	return StringView(Buffer[GetCurrentBufferIndexAndIncrement()], 6);
}

StringView AnsiString::ConvertParam(Format::__HexUint24 v) {
	char8* t = Buffer[CurrentBuffer];
	*t++ = '#';
	
	ConvertOneHexDigit((v.value >> 16) & 0xFF, t);
	ConvertOneHexDigit((v.value >> 8) & 0xFF, t);
	ConvertOneHexDigit(v.value & 0xFF, t);
	return StringView(Buffer[GetCurrentBufferIndexAndIncrement()], 7);
}

StringView AnsiString::ConvertParam(Format::__HexUint32 v) {
	char8* t = Buffer[CurrentBuffer];
	*t++ = '0';
	*t++ = 'x';
	
	for (uint32 i = 0; i < 4; ++i) {
		ConvertOneHexDigit((v.value >> (24 - i * 8)) & 0xFF, t);
	}
	return StringView(Buffer[GetCurrentBufferIndexAndIncrement()], 10);
}

StringView AnsiString::ConvertParam(Format::__HexUint64 v) {
	char8* t = Buffer[CurrentBuffer];
	*t++ = '0';
	*t++ = 'x';
	
	for (uint32 i = 0; i < 8; ++i) {
		ConvertOneHexDigit((v.value >> (56 - i * 8)) & 0xFF, t);
	}
	return StringView(Buffer[GetCurrentBufferIndexAndIncrement()], 18);
}