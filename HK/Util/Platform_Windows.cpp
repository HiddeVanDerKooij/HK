#include "Common/CompilerMacros.h"

#if PLATFORM == PLATFORM_WINDOWS

#include "Platform.h"

#include <Windows.h>

void Platform::SetDPIAware() {
	SetProcessDPIAware();
}

uint64 Platform::GetTicks() noexcept {
	LARGE_INTEGER li;
	QueryPerformanceCounter(&li);
	return li.QuadPart;
}

uint64 Platform::GetFrequency() noexcept {
	LARGE_INTEGER li;
	QueryPerformanceFrequency(&li);
	return li.QuadPart;
}

#endif