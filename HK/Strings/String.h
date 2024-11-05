// Copyright (c) 2023-2024, Hidde van der Kooij
// SPDX-License-Identifier: BSD-2-Clause

#pragma once

#include "Common/Types.h"
#include "Containers/Array.h"
#include "Containers/View/StringView.h"

// TODO (HvdK): UTF-8

struct Format
{
public:
	enum class FormatType : uint8
	{
		Decimal,
		Hex,
		HexColor,
		Binary,
	};
	
	uint64 Value;
	FormatType Type;
	uint8 DesiredLength;
	char8 PaddingCharPrepend;
	char8 PaddingCharAppend;
	
	Format(FormatType type, uint8 desiredLength, char8 paddingCharPrepend, char8 paddingCharAppend, uint64 value)
		: Value(value), Type(type), DesiredLength(desiredLength), PaddingCharPrepend(paddingCharPrepend), PaddingCharAppend(paddingCharAppend) {}
	
public:
	static Format AsHex8(uint8 value) {
		return Format(FormatType::Hex, 1, '0', '\0', value);
	}
	static Format AsHex16(uint16 value) {
		return Format(FormatType::Hex, 2, '0', '\0', value);
	}
	static Format AsHex24(uint32 value) {
		return Format(FormatType::Hex, 3, '0', '\0', value);
	}
	static Format AsHex32(uint32 value) {
		return Format(FormatType::Hex, 4, '0', '\0', value);
	}
	static Format AsHex64(uint64 value) {
		return Format(FormatType::Hex, 8, '0', '\0', value);
	}
	static Format AsHexColor3(uint32 value) {
		return Format(FormatType::HexColor, 3, '0', '\0', value);
	}
	static Format AsHexColor4(uint32 value) {
		return Format(FormatType::HexColor, 4, '0', '\0', value);
	}
	static Format AsPointer(const void* value) {
		return Format(FormatType::Hex, sizeof(void*)>>1, '0', '\0', uint64(value));
	}
};

class AnsiString : protected Array<char8> {
public:
	AnsiString();
	AnsiString(const AnsiString& copy);
	AnsiString(AnsiString&& move);
	AnsiString(char8 c);
	AnsiString(uint32 buffer);
	AnsiString(StringView);

	char8 At(uint32 index) const;

	AnsiString& operator=(const AnsiString& other);
	AnsiString& operator=(AnsiString&& other);
	AnsiString& operator=(StringView view);
	AnsiString& operator+=(const AnsiString& other);
	AnsiString& operator+=(StringView other);
	AnsiString& operator+=(const char8* other);
	AnsiString& operator+=(char8 c);
	AnsiString operator+(const AnsiString& other);
	AnsiString operator+(StringView other);
	AnsiString operator+(const char8* other);
	const char8* operator*();
	const char8* operator*() const;
	bool operator==(const AnsiString& other) const;
	bool operator!=(const AnsiString& other) const;
	bool operator==(const StringView& other) const;
	bool operator!=(const StringView& other) const;
	
	void RemoveLast(uint32 n);
	
	uint32 Size() const;

	void Append(StringView view);

	StringView AsView() const;
	const char8* AsCString();
	const char8* AsCString() const;

	void Trim();

	static AnsiString Repeated(const char8 c, uint32 count);
	
	bool ParseAsInt(int64& result, uint32 base) const;

protected:
	void EnsureNullTerminated();
	void CheckNullTerminated() const;

public:
	template<class... Types>
	static AnsiString Format(StringView format, const Types&... params) {
		AnsiString str(format.Size() + 8);
		uint32 offset = 0;
		FormatImpl(str, format, offset, params...);
		FormatImpl(str, format, offset);
		return str;
	};

protected:
	template<typename U, class... Types>
	static void FormatImpl(AnsiString& str, StringView format, uint32& offset, const U& param, Types... params) {
		FormatImpl(str, format, offset, param);
		FormatImpl(str, format, offset, params...);
	};
	template<typename U>
	static void FormatImpl(AnsiString& str, StringView format, uint32& offset, const U& param) {
		int32 pos = format.LeftFind('{', offset);
		if (pos == -1) {
			return FormatImpl(str, format, offset);
		}
		str += format.Substring(offset, pos - offset);
		str.Append(ConvertParam(param));
		pos = format.LeftFind('}', pos);
		CHECK(pos >= 0);
		++pos;
		offset = pos;
	}
	static void FormatImpl(AnsiString& str, StringView format, uint32 offset) {
		str += format.Substring(offset, format.Size() - offset);
	}

	static StringView ConvertParam(const AnsiString& v);
	static StringView ConvertParam(StringView v);
	static StringView ConvertParam(const char8* v);
	static StringView ConvertParam(int8 v);
	static StringView ConvertParam(uint8 v);
	static StringView ConvertParam(int16 v);
	static StringView ConvertParam(uint16 v);
	static StringView ConvertParam(int32 v);
	static StringView ConvertParam(uint32 v);
	static StringView ConvertParam(int64 v);
	static StringView ConvertParam(uint64 v);
	static StringView ConvertParam(bool v);
	static StringView ConvertParam(const void* v);
	static StringView ConvertParam(f32 v);
	static StringView ConvertParam(f64 v);
	
	static StringView ConvertParam(const ::Format& v);
};

typedef AnsiString String;