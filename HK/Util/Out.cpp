// Copyright (c) 2023-2024, Hidde van der Kooij
// SPDX-License-Identifier: BSD-2-Clause

#include "Out.h"

#include "Common/CompilerMacros.h"

// TODO (HvdK): Cleanup

#ifdef _DEBUG_

#if PLATFORM == PLATFORM_WINDOWS
#include <Windows.h>
#define PRINT(x) OutputDebugString(x)
#else
#include <iostream>
#define PRINT(x) std::cout << x
#endif

#else

#define PRINT(x)

#endif

void Out::Write(StringView str) {
	WriteImpl(str, false);
}

void Out::Write(const String& str) {
	WriteImpl(str.AsView(), false);
}

void Out::WriteLine(StringView str) {
	WriteImpl(str, true);
}

void Out::WriteLine(const String& str) {
	WriteImpl(str.AsView(), true);
}

void Out::WriteEnvironment() {
	WriteLine("HK: {}"_sv, BUILD_STRING);
}

void Out::WriteImpl(StringView str, bool bNewLine) {

	String output = String();

	if (bNewLine) {
		output = String::Format("{}\n"_sv, str);
	}
	else {
		output = String::Format("{}"_sv, str);
	}
	
	PRINT(output.AsCString());
}