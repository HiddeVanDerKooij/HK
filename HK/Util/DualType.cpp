#include "DualType.h"
#include "Common/CompilerMacros.h"

GDualType::GDualType() : state(0)
{
}

GDualType::GDualType(const GDualType& other)
{
	state = other.state;
}

void GDualType::CheckCorrect(bool b1) const
{
	ASSERT(state == (b1? 1 : 2));
}

bool GDualType::Is1() const
{
	return state == 1;
}

bool GDualType::Is2() const
{
	return state == 2;
}