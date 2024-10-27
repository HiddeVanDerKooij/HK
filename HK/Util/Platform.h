#pragma once

#include "Common/Types.h"

namespace Platform {
	void SetDPIAware();
	// TODO (HvdK): noexcept everywhere?
	uint64 GetTicks() noexcept;
	uint64 GetFrequency() noexcept;
}