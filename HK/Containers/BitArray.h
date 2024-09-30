// Copyright (c) 2024, Hidde van der Kooij
// SPDX-License-Identifier: BSD-2-Clause

#pragma once

#include "Containers/Array.h"

class BitArray : protected Array<uint8> {
public:
	BitArray();
	BitArray(uint32 bitsbuffer);
	BitArray(const BitArray& other);
	BitArray(BitArray&& other);
	~BitArray();
	
	void AddBit(bool bit);
	bool GetBit(uint32 index) const;
	
	uint32 GetBitCount() const;
	uint32 GetByteCount() const;
	
	void Reset();
	bool IsValidIndex(uint32 index) const;
	
	bool operator[](uint32 index) const;
	
	BitArray& operator=(const BitArray& other);
	BitArray& operator=(BitArray&& other);
	
protected:
	uint32 BitNum;
};