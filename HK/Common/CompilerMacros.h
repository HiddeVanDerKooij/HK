#pragma once

#define AssertIfFailed(success) {if (!(success)) { BREAK; }}

// Normalizing definitions passed into the build system

#ifdef _DEBUG_
#define DEBUG_BUILD
#define BUILD_TYPE "DEBUG"
#endif

#ifdef _RELEASE_
#define RELEASE_BUILD
#define BUILD_TYPE "RELEASE"
#endif

#ifdef _MSC_VER
#define MSVC
#define COMPILER_VENDOR "MSVC"
#endif

#ifdef __GNUC__
#define GCC
#define COMPILER_VENDOR "GCC"
#endif

// Compiler specific macros


#ifdef MSVC

// TODO (HvdK): Move the dependency of this header
// to the CPP file, by only defining a stub here and
// have the linker resolve it.
#include <intrin.h>

#define __LIKELY_IMPL(x) x
#define __UNLIKELY_IMPL(x) x
#define __RESTRICT_IMPL(x) __restrict
#define __ASSUME_IMPL(x) __assume(LIKELY(x))
#define __BREAK_IMPL {__nop(); __debugbreak();}
#define __PREFETCH_READ_IMPL(x) _mm_prefetch((const char*)(x), _MM_HINT_T0)
#define __PREFETCH_WRITE_IMPL(x) _mm_prefetch((const char*)(x), _MM_HINT_T1)
#define __UNREACHABLE_IMPL __assume(0)
#define __PACK_START_IMPL __pragma(pack(push,1))
#define __PACK_END_IMPL __pragma(pack(pop))

#endif

#ifdef GCC

#define __LIKELY_IMPL(x) __builtin_expect(!!(x), 1)
#define __UNLIKELY_IMPL(x) __builtin_expect(!!(x), 0)
#define __RESTRICT_IMPL(x) __restrict__
#define __ASSUME_IMPL(x) do { if (!(LIKELY(x))) __builtin_unreachable(); } while (0)
#define __BREAK_IMPL __builtin_trap()
#define __PREFETCH_READ_IMPL(x) __builtin_prefetch((const void*)(x), 0, 1)
#define __PREFETCH_WRITE_IMPL(x) __builtin_prefetch((const void*)(x), 1, 1)
#define __UNREACHABLE_IMPL __builtin_unreachable()
#define __PACK_START_IMPL __attribute__((packed))

#endif

// Blank defaults for compilers that don't support the above

#ifndef __LIKELY_IMPL
#define __LIKELY_IMPL(x) x
#endif
#ifndef __UNLIKELY_IMPL
#define __UNLIKELY_IMPL(x) x
#endif
#ifndef __RESTRICT_IMPL
#define __RESTRICT_IMPL(x) x
#endif
#ifndef __ASSUME_IMPL
#define __ASSUME_IMPL(x) {}
#endif
#ifndef __BREAK_IMPL
#define __BREAK_IMPL {}
#endif
#ifndef __PREFETCH_READ_IMPL
#define __PREFETCH_READ_IMPL(x) {}
#endif
#ifndef __PREFETCH_WRITE_IMPL
#define __PREFETCH_WRITE_IMPL(x) {}
#endif
#ifndef __UNREACHABLE_IMPL
#define __UNREACHABLE_IMPL {}
#endif
#ifndef __PACK_START_IMPL
#define __PACK_START_IMPL {}
#endif
#ifndef __PACK_END_IMPL
#define __PACK_END_IMPL {}
#endif
#ifndef __DBG_INCREMENT_IMPL
#define __DBG_INCREMENT_IMPL(x) {}
#endif
#ifndef __ASSERT_IMPL
#define __ASSERT_IMPL(x) AssertIfFailed(LIKELY(x))
#endif

#ifndef BUILD_TYPE
#define BUILD_TYPE "UNKNOWN"
#endif
#ifndef COMPILER_VENDOR
#define COMPILER_VENDOR "UNKNOWN"
#endif

// Debug specific macros

#ifdef DEBUG_BUILD

#undef __ASSUME_IMPL
#define __ASSUME_IMPL(x)			AssertIfFailed(LIKELY(x))
#undef __DBG_INCREMENT_IMPL
#define __DBG_INCREMENT_IMPL(x)	++(x)
#undef __UNREACHABLE_IMPL
#define __UNREACHABLE_IMPL			AssertIfFailed(false)

#endif


// Special macros built from native components
#define CHECK(x)				ASSERT(x)
#define STATIC_CHECK(x)			static_assert(x, #x)
#define __BS                    CONCATENATE(COMPILER_VENDOR, "_")
#define BUILD_STRING			(BUILD_TYPE "_" COMPILER_VENDOR)
#define STR(x)					#x
#define XSTR(x)					STR(x)
#define CONCATENATE(x, y)		x##y


// Release specific macros

#define ASSERT(x)            __ASSERT_IMPL(x)
#define LIKELY(x)			__LIKELY_IMPL(x)
#define UNLIKELY(x)			__UNLIKELY_IMPL(x)
#define RESTRICT(x)			__RESTRICT_IMPL(x)
#define ASSUME(x)			__ASSUME_IMPL(x)
#define BREAK				__BREAK_IMPL
#define PREFETCH_READ(x)	__PREFETCH_READ_IMPL(x)
#define PREFETCH_WRITE(x)	__PREFETCH_WRITE_IMPL(x)
#define UNREACHABLE			__UNREACHABLE_IMPL
#define PACK_START			__PACK_START_IMPL
#define PACK_END			__PACK_END_IMPL
#define DBG_INCREMENT(x)	__DBG_INCREMENT_IMPL(x)

#define ARRAY_COUNT(x)		(sizeof(x) / sizeof(x[0]))