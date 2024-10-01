// Copyright (c) 2024, Hidde van der Kooij
// SPDX-License-Identifier: BSD-2-Clause

#include "Common/CompilerMacros.h"

#if PLATFORM == PLATFORM_WINDOWS
#include "FilePath.h"

char8 FilePathStatics::GetPlatformPathSeparator()
{
	return '\\';
}

bool FilePathStatics::IsAbsolutePlatform(StringView path)
{
	if (path.Size() > 2)
	{
		if (path.At(1) == ':')
		{
			return true;
		}
	}
	return false;
}

#endif