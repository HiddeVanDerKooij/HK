// Copyright (c) 2023-2024, Hidde van der Kooij
// SPDX-License-Identifier: BSD-2-Clause

#pragma once

#include "Common/Types.h"
#include "Strings/String.h"

class Out {
public:
	template<class... Types>
	static void Write(StringView format, Types... args)
	{
		String str = String::Format(format, args...);
		WriteImpl(str.AsView(), false);
	}
	
	template<class... Types>
	static void WriteLine(StringView format, Types... args)
	{
		String str = String::Format(format, args...);
		WriteImpl(str.AsView(), true);
	}
	
	static void Write(StringView str);
	static void Write(const String& str);
	static void WriteLine(StringView str);
	static void WriteLine(const String& str);


private:
	static void WriteImpl(StringView str, bool bNewLine);
};