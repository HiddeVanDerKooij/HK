#include "Strings/String.h"

#include "Allocators/Memory.h"
#include "Common/CompilerMacros.h"

uint8 CurrentBuffer = 0;
char8 Buffer[16][256] = { '\0' };

Format::__HexUint16 Format::AsHex(uint16 value) {
    return *reinterpret_cast<__HexUint16*>(&value);
}

AnsiString::AnsiString() : Array<char8>() {}

AnsiString::AnsiString(const AnsiString& copy) : Array<char8>(copy.ArrayMax) {
    if (LIKELY(copy.ArrayNum > 0)) {
        ArrayNum = copy.ArrayNum;
        Memory::Copy(copy.Data, Data, ElementSize * (ArrayNum));
	}
}

AnsiString::AnsiString(AnsiString&& move) : Array<char8>(Move(move)) {}

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
        RequireArrayMaxGrowth(other.ArrayNum);
        Memory::Copy(other.Data, Data, ElementSize * (other.ArrayNum));
        ArrayNum = other.ArrayNum;
    }
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

const char8* AnsiString::operator*() {
    return AsCString();
}

const char8* AnsiString::operator*() const {
    return AsCString();
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
    if (UNLIKELY(Data == nullptr))
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
    return StringView(Buffer[CurrentBuffer++], l);
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

StringView AnsiString::ConvertParam(Format::__HexUint16 v) {
    char8* t = Buffer[CurrentBuffer];
    *t++ = '0';
    *t++ = 'x';
    
    uint16 m = 0b1111;
    
    const char8* hex = "0123456789ABCDEF";
    
    for (uint32 i = 0; i < 4; ++i) {
        uint16 cm = m << ((3 - i) * 4);
        uint16 c = (v.value & cm) >> ((3-i) * 4);
        *t++ = hex[c];
    }
    return StringView(Buffer[CurrentBuffer++], 6);
}