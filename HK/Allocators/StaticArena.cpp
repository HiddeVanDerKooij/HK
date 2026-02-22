#include "StaticArena.h"

GStaticArena::Scope::Scope(GStaticArena& arena)
	: Arena(arena), Offset(arena.Offset)
{}

GStaticArena::Scope::~Scope()
{
	Arena.Offset = Offset;
}

uint32 GStaticArena::GetOffset() const
{
	return Offset;
}
void GStaticArena::Reset()
{
	Offset = 0;
}
GStaticArena::Scope GStaticArena::CreateScope()
{
	return Scope(*this);
}