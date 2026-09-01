// Copyright (c) 2023-2024, Hidde van der Kooij
// SPDX-License-Identifier: BSD-2-Clause

#include "Common/CompilerMacros.h"

// TODO (HvdK): Cleanup

#ifdef _DEBUG_

#if PLATFORM == PLATFORM_WINDOWS
#include <Windows.h>
#define PRINT(x) OutputDebugString(x)
#include <iostream>
#else
#include <iostream>
#define PRINT(x) std::cout << x
#endif

#else

#define PRINT(x)

#endif

// Moved below to avoid including Windows.h in the header file, namely
// re-defining the (placement) new.
#include "Out.h"

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
	// TODO (HvdK): Optimize
	String output = String(str.Size() + 3);

	if (bNewLine) {
		output = String::Format("{}\n"_sv, str);
	}
	else {
		output = String::Format("{}"_sv, str);
	}
	
	PRINT(output.AsCString());
}