#pragma once

#include "Common/Types.h"

class GDualType {
protected:
	GDualType();
	GDualType(const GDualType& other);
	void CheckCorrect(bool b1) const;
	bool Is1() const;
	bool Is2() const;

protected:
	uint8 state;
};

// This class is used to store two types as a union.
template<typename T1, typename T2>
class DualType : public GDualType {
public:
	static DualType MakeT1(const T1& value) {
		DualType result;
		result.Value1 = value;
		result.state = 1;
		return result;
	}

	static DualType MakeT2(const T2& value) {
		DualType result;
		result.Value2 = value;
		result.state = 2;
		return result;
	}

protected:
	DualType() : GDualType() {}
	DualType(const DualType& other) : GDualType(other) {
		if (other.Is1()) {
			Value1 = other.Value1;
		}
		else if (other.Is2()) {
			Value2 = other.Value2;
		}
	}

public:
	T1& Get1() { CheckCorrect(true);	return Value1; }
	T2& Get2() { CheckCorrect(false);	return Value2; }
	const T1& Get1() const { CheckCorrect(true);	return Value1; }
	const T2& Get2() const { CheckCorrect(false);	return Value2; }

protected:
	union {
		T1 Value1;
		T2 Value2;
	};
};