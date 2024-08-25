#include "Common/CompilerMacros.h"
#include "Common/Types.h"

STATIC_CHECK(sizeof(int64) == 8);
STATIC_CHECK(sizeof(uint64) == 8);
STATIC_CHECK(sizeof(int32) == 4);
STATIC_CHECK(sizeof(uint32) == 4);
STATIC_CHECK(sizeof(int16) == 2);
STATIC_CHECK(sizeof(uint16) == 2);
STATIC_CHECK(sizeof(int8) == 1);
STATIC_CHECK(sizeof(uint8) == 1);
STATIC_CHECK(sizeof(size64) == 8);