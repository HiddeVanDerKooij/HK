// Copyright (c) 2023-2024, Hidde van der Kooij
// SPDX-License-Identifier: BSD-2-Clause

#pragma once

#include "Common/Types.h"
#include "Containers/Array.h"
#include "Containers/View/StringView.h"

// TODO (HvdK): UTF-8

namespace Format {
    struct __HexUint8 { uint8 value; };
    __HexUint8 AsHex8(uint8 value);
    struct __HexUint16 { uint16 value; };
    __HexUint16 AsHex16(uint16 value);
    struct __HexUint24 { uint32 value; };
    __HexUint24 AsHex24(uint32 value);
    struct __HexUint32 { uint32 value; };
    __HexUint32 AsHex32(uint32 value);
    struct __HexUint64 { uint64 value; };
    __HexUint64 AsHex64(uint64 value);
    __HexUint64 AsHex64(const void* value);
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
    
    static StringView ConvertParam(Format::__HexUint8 v);
    static StringView ConvertParam(Format::__HexUint16 v);
    static StringView ConvertParam(Format::__HexUint24 v);
    static StringView ConvertParam(Format::__HexUint32 v);
    static StringView ConvertParam(Format::__HexUint64 v);
};

typedef AnsiString String;