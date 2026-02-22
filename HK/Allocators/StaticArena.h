#pragma once

#include "Common/CompilerMacros.h"
#include "Common/Types.h"

struct GStaticArena
{
public:
	// This is a scope that when it is out of scope
	// will reset the offset of the arena.
	struct Scope
	{
		Scope(GStaticArena& arena);
		~Scope();
		GStaticArena& Arena;
		uint32 Offset;
	};
	
	uint32 GetOffset() const;
	void Reset();
	Scope CreateScope();
	
	uint32 Offset = 0;

protected:
	GStaticArena() = default;
};

// A static arena has a compile-time size that is
// allocated with the instance of the arena.
// You can grow, shrink, and reset, but you may not
// allocate more than the size of the arena.
template<int _Size>
struct StaticArena : public GStaticArena
{
public:
	static_assert(_Size > 0, "StaticArena size must be greater than 0");
	static const uint32 Size = uint32(_Size);

	StaticArena() = default;
	
	template<typename T>
	T* Allocate()
	{
		CHECK(Offset + sizeof(T) <= Size);
		T* ptr = (T*)&Data[Offset];
		Offset += sizeof(T);
		return ptr;
	}
	
	template<typename T>
	T* Allocate(T&& value)
	{
		T* object = Allocate<T>();
		*object = value;
		return object;
	}
	
	void SetOffset(uint32 offset)
	{
		CHECK(offset <= Size);
		Offset = offset;
	}
	
protected:
	uint8 Data[Size];
};