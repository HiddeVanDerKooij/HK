// memmove_s, memcpy_s
#include <cstring>

// malloc
#include <cstdlib>

#include "Allocators/Memory.h"
#include "Common/CompilerMacros.h"
#include "Common/Types.h"
#include "Containers/View/StringView.h"

void* Memory::Allocate(uint64 numBytes)
{
	void* data = malloc(numBytes);
	CHECK(data != nullptr);

#ifdef DEBUG_BUILD
	memset(data, 0xCD, numBytes);
#endif

	return data;
}

void Memory::Reallocate(void*& ptr, uint64 oldNumBytes, uint64 newNumBytes)
{
	CHECK(ptr != nullptr);
	ptr = realloc(ptr, newNumBytes);
	CHECK(ptr != nullptr);

#ifdef DEBUG_BUILD
	if (newNumBytes > oldNumBytes) {
		memset((uint8*)ptr + oldNumBytes, 0xDE, newNumBytes - oldNumBytes);
	}
#endif
}

void Memory::Free(void* ptr, uint64 numBytes)
{
	CHECK(ptr != nullptr);
	free(ptr);
}

void Memory::FillZero(void* ptr, uint64 numBytes)
{
	CHECK(ptr != nullptr);
	
	// TODO (HvdK): First fill to alignment, then do this
	
	char8* cptr = reinterpret_cast<char8*>(ptr);
	
	while (numBytes >= 32)
	{
		uint64* ptr64 = reinterpret_cast<uint64*>(cptr);
		ptr64[0] = 0;
		ptr64[1] = 0;
		ptr64[2] = 0;
		ptr64[3] = 0;
		cptr += 32;
		numBytes -= 32;
	}
	while (numBytes >= 8)
	{
		uint64* ptr64 = reinterpret_cast<uint64*>(cptr);
		*ptr64 = 0;
		cptr += 8;
		numBytes -= 8;
	}
	if (numBytes >= 4)
	{
		uint32* ptr32 = reinterpret_cast<uint32*>(cptr);
		*ptr32 = 0;
		cptr += 4;
		numBytes -= 4;
	}
	if (numBytes >= 2)
	{
		uint16* ptr16 = reinterpret_cast<uint16*>(cptr);
		*ptr16 = 0;
		cptr += 2;
		numBytes -= 2;
	}
	if (numBytes == 1)
	{
		*cptr = 0;
		--numBytes;
	}
	
	CHECK(numBytes == 0);
}

void Memory::Move(const void* from, void* to, uint64 numBytes)
{
#ifdef MSVC
	memmove_s(to, numBytes, from, numBytes);
#else
	memmove(to, from, numBytes);
#endif
}

void Memory::Copy(const void* from, void* to, uint64 numBytes)
{
#ifdef MSVC
	memcpy_s(to, numBytes, from, numBytes);
#else
	memcpy(to, from, numBytes);
#endif
}

bool Memory::Equals(const void* a, const void* b, uint64 numBytes)
{
	const uint8* a8 = (const uint8*)a;
	const uint8* b8 = (const uint8*)b;

	while (numBytes >= 8) {
		uint64 a64 = *(uint64*)a8;
		uint64 b64 = *(uint64*)b8;
		if (a64 != b64) {
			return false;
		}
		a8 += 8;
		b8 += 8;
		numBytes -= 8;
	}
	while (numBytes >= 4) {
		uint32 a32 = *(uint32*)a8;
		uint32 b32 = *(uint32*)b8;
		if (a32 != b32) {
			return false;
		}
		a8 += 4;
		b8 += 4;
		numBytes -= 4;
	}
	while (numBytes >= 2) {
		uint16 a16 = *(uint16*)a8;
		uint16 b16 = *(uint16*)b8;
		if (a16 != b16) {
			return false;
		}
		a8 += 2;
		b8 += 2;
		numBytes -= 2;
	}
	if (numBytes == 1) {
		return *a8 == *b8;
	}
	else {
		return true;
	}
}

bool Memory::StringEquals(const char8* a, const char8* b)
{
	// TODO: Use a faster implementation
	return strcmp(a, b) == 0;
}

bool Memory::StringEquals(const char8* a, const char8* b, uint32 size)
{
	return Memory::Equals(a, b, size);
}

bool Memory::StringEquals(const StringView& a, const StringView& b)
{
	if (a.Size() != b.Size())
		return false;
	return Memory::Equals(a.Data(), b.Data(), a.Size());
}

uint32 Memory::StringSize(const char8* string)
{
	return (uint32)strlen(string);
}