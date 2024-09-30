// Copyright (c) 2024, Hidde van der Kooij
// SPDX-License-Identifier: BSD-2-Clause

#include "Containers/BitArray.h"

#include "Allocators/Memory.h"

BitArray::BitArray() : Array<uint8>() {
	BitNum = 0;
}
BitArray::BitArray(uint32 bitsbuffer) : Array<uint8>(bitsbuffer / 8 + 1) {
	BitNum = 0;
}

BitArray::BitArray(const BitArray& other) : Array<uint8>(other) {
	BitNum = other.BitNum;
}

BitArray::BitArray(BitArray&& other) : Array<uint8>(Move(other)) {
	BitNum = other.BitNum;
	other.BitNum = 0;
}

BitArray::~BitArray() {
}

void BitArray::AddBit(bool bit) {
	if (UNLIKELY(BitNum / 8 >= ArrayNum))
		uint8* discard = AddUninitialized(1);
	
	uint32 byteIndex = BitNum / 8;
	uint32 bitIndex = BitNum % 8;
	
	++BitNum;
	
	if (bit) {
		Data[byteIndex] |= 1 << bitIndex;
	} else {
		Data[byteIndex] &= ~(1 << bitIndex);
	}
}

bool BitArray::GetBit(uint32 index) const {
	uint32 byteIndex = index / 8;
	uint32 bitIndex = index % 8;
	
	CHECK(index < BitNum);
	CHECK(byteIndex < ArrayNum);
	
	return (Data[byteIndex] & (1 << bitIndex)) != 0;
}

uint32 BitArray::GetBitCount() const {
	return BitNum;
}

uint32 BitArray::GetByteCount() const {
	return ArrayNum;
}

void BitArray::Reset() {
	ArrayNum = 0;
	BitNum = 0;
}

bool BitArray::IsValidIndex(uint32 index) const {
	return index < BitNum;
}

bool BitArray::operator[](uint32 index) const {
	return GetBit(index);
}

BitArray& BitArray::operator=(const BitArray& other) {
	Array<uint8>::operator=(other);
	BitNum = other.BitNum;
	return *this;
}

BitArray& BitArray::operator=(BitArray&& other) {
	Array<uint8>::operator=(Move(other));
	BitNum = other.BitNum;
	other.BitNum = 0;
	return *this;
}