// Copyright (c) 2023-2024, Hidde van der Kooij
// SPDX-License-Identifier: BSD-2-Clause

#pragma once

#include "Common/Types.h"
#include "Containers/View/StringView.h"
#include "Strings/String.h"

class StringUtil
{
public:
	static bool IsWhitespace(char8 c);
	static char8 ToLower(char8 c);
	static char8 ToUpper(char8 c);
	
	template<typename A, typename B>
	static bool IsEqual(const A& a, const B& b) {
		return IsEqualImpl(AsStringView(a), AsStringView(b));
	}
	
	template<typename A, typename B>
	static bool IsEqualCaseInsensitive(const A& a, const B& b) {
		return IsEqualCaseInsensitiveImpl(AsStringView(a), AsStringView(b));
	}
	
	static StringView AsStringView(const StringView&);
	static StringView AsStringView(const char8*);
	static StringView AsStringView(const String&);
	
protected:
	static bool IsEqualImpl(const StringView& a, const StringView& b);
	static bool IsEqualCaseInsensitiveImpl(const StringView& a, const StringView& b);
};