// Copyright (c) 2023-2024, Hidde van der Kooij
// SPDX-License-Identifier: BSD-2-Clause

#include "CompilerMacros.h"

#include "Util/Out.h"

void AssertFailed(const char* file, int line, const char* code)
{
	Out::WriteLine("Assertion failed: {}:{}: '{}'"_sv, file, line, code);
}
