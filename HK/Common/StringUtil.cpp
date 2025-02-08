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