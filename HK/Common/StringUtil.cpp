// Copyright (c) 2023-2024, Hidde van der Kooij
// SPDX-License-Identifier: BSD-2-Clause

#include "Common/StringUtil.h"

#include "Common/CompilerMacros.h"

bool StringUtil::IsWhitespace(char8 c)
{
	if (c == ' ') return true;
	if (UNLIKELY(c == '\t') || c == '\n' || c == '\r') return true;
	return false;
}

char8 StringUtil::ToLower(char8 c)
{
	if (c >= 'A' && c <= 'Z') return c - 'A' + 'a';
	return c;
}

char8 StringUtil::ToUpper(char8 c)
{
	if (c >= 'a' && c <= 'z') return c - 'a' + 'A';
	return c;
}

StringView StringUtil::AsStringView(const StringView& view)
{
	return view;
}

StringView StringUtil::AsStringView(const char8* string)
{
	return StringView(string);
}

StringView StringUtil::AsStringView(const String& s)
{
	return s.AsView();
}

bool StringUtil::IsEqualImpl(const StringView& a, const StringView& b)
{
	return Memory::StringEquals(a, b);
}

bool StringUtil::IsEqualCaseInsensitiveImpl(const StringView& a, const StringView& b)
{
	if (a.Size() != b.Size()) return false;
	if (a.Data() == b.Data()) return true;
	
	// TODO (HvdK): UTF-8
	
	for (uint32 i=0; i<a.Size(); ++i)
	{
		if (ToLower(a.At(i)) != ToLower(b.At(i))) return false;
	}
	return true;
}