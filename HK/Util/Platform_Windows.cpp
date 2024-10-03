#include "Common/CompilerMacros.h"

#if PLATFORM == PLATFORM_WINDOWS

#include "Platform.h"

#include <Windows.h>

void Platform::SetDPIAware() {
	SetProcessDPIAware();
}

#endif