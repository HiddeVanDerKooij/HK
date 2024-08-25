#include <cassert>

#include "CompilerMacros.h"

void AssertIfFailed(bool bSuccess)
{
	if (bSuccess)
		return;
	BREAK;
}