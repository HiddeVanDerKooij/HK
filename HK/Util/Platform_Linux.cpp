#include "Common/CompilerMacros.h"

#if PLATFORM == PLATFORM_LINUX

#include "Platform.h"

#include <time.h>

void Platform::SetDPIAware() {
	// Do nothing
}

uint64 Platform::GetTicks() noexcept {
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return static_cast<uint64>(ts.tv_sec) * 1000000000ULL + static_cast<uint64>(ts.tv_nsec);
}

uint64 Platform::GetFrequency() noexcept {
	return 1000000000ULL;
}

#endif