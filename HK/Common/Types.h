#pragma once

// TODO (HvdK): G++ wants this file here
#include <stddef.h>

typedef long long int64;
typedef unsigned long long uint64;
typedef int int32;
typedef unsigned int uint32;
typedef short int16;
typedef unsigned short uint16;
typedef char int8;
typedef unsigned char uint8;

typedef int8 char8;

typedef size_t size64;

constexpr uint32 INVALID_INDEX = 0xFFFFFFFF;

enum ENoInit { NoInit };

typedef char8 nil;