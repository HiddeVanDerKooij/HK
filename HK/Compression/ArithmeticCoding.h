// Copyright (c) 2024, Hidde van der Kooij
// SPDX-License-Identifier: BSD-2-Clause

#pragma once

#include "Common/CompilerMacros.h"
#include "Common/Types.h"
#include "Containers/Array.h"
#include "Containers/View/ArrayView.h"
#include "Containers/BitArray.h"


// Our encoding and decoding requires a model that is an instance of class T,
// however you may want to have a model that only has static functions.
// You may use this wrapper class to make it an instance.
template <typename T>
class ArithmeticCodingModelInstance {
public:
	uint32 GetSymbolCount() const {
		return T::GetSymbolCount();
	};
	
	f32 GetSymbolProbability(const Array<uint32>& encoded, uint32 symbol) const {
		return T::GetSymbolProbability(encoded, symbol);
	};
	
	f32 GetTerminatorSymbolProbability(const Array<uint32>& encoded, uint32 context) const {
		return T::GetTerminatorSymbolProbability(encoded, context);
	};
};

// The template T must be a class that has the following methods:
// - T::GetSymbolCount() -> uint32
// - T::GetSymbolProbability(ArrayView<V> encoded, uint32 symbol) -> f32
// - T::GetTerminatorSymbolProbability(ArrayView<V> encoded, uint32 context) -> f32
// The template V is the type of the values that will be encoded/decoded
template <typename T, typename V>
class ArithmeticCoding {
public:
	static BitArray Encode(const ArrayView<V>& values, const T& model) {
		
		struct TargetSymbol {
			f32 Start;
			f32 End;
		};
		
		// We first generate all our target symbols, so min/max pairings
		Array<TargetSymbol> targetSymbols;
		targetSymbols.Reserve(values.Size()+1);
		for (uint32 i=0; i<=values.Size(); ++i) {
			TargetSymbol& symbol = targetSymbols.AddRef(TargetSymbol());
			
			symbol = [&](uint32 pos)
			{
				TargetSymbol output;
				uint32 symbolIndex = 0xFFFFFFFF;
				if (LIKELY(pos < values.Size())) {
					symbolIndex = model.ToIndex(values[pos]);
				}
				
				const ArrayView<V> context = ArrayView(values.ConstData(), pos);
				
				const f32 terminatorProbability = model.GetTerminatorSymbolProbability(context);
				CHECK(terminatorProbability >= 0.f);
				
				f32 totalProbability = terminatorProbability;
				const uint32 symbolCount = model.GetSymbolCount();
				for (uint32 i=symbolCount; i>0;)
				{
					--i;
					f32 probability = model.GetSymbolProbability(context, i);
					CHECK(probability>=0.f);
					CHECK(probability < 100000.f);
					totalProbability += probability;
				}
				
				f32 start = 0.f;
				
				for (uint32 i=symbolCount; i>0;)
				{
					--i;
					f32 probability = model.GetSymbolProbability(context, i);
					if (probability == 0.f)
						continue;
				
					if (UNLIKELY(i == symbolIndex)) {
						output.Start = start / totalProbability;
						output.End = (start + probability) / totalProbability;
						return output;
					}
					start += probability;
				}
			
				CHECK(terminatorProbability > 0.f);
				output.Start = start / totalProbability;
				output.End = 1.f;
				return output;
			}(i);
		}
		
		uint32 rangeMin = 0x00000000;
		uint32 rangeMax = 0xFFFFFFFF;
		uint32 underflowBits = 0;
		
		BitArray encoded;
		
		for (int32 i=0; i<targetSymbols.Num(); ++i)
		{
			const auto& symbol = targetSymbols[i];
			
			uint64 range = uint64(rangeMax) - uint64(rangeMin) + 1;
			rangeMax = rangeMin + uint32(range * symbol.End) - 1;
			rangeMin = rangeMin + uint32(range * symbol.Start);
			
			while (1)
			{
				if ((rangeMax & 0x80000000) == (rangeMin & 0x80000000))
				{
					// Output the MSB and shift
					encoded.AddBit((rangeMin >> 31) & 1);
					rangeMin = (rangeMin << 1) & 0xFFFFFFFF;
					rangeMax = ((rangeMax << 1) | 1) & 0xFFFFFFFF;
				}
				else if ((rangeMin & 0x40000000) && !(rangeMax & 0x40000000))
				{
					// Handle underflow
					rangeMin = (rangeMin << 1) & 0x3FFFFFFF;
					rangeMax = (rangeMax << 1) | 0x80000000;
				}
				else
				{
					break;
				}
			}
		}
		
		// Finalizing
		while ((rangeMin & 0x80000000) == (rangeMax & 0x80000000))
		{
			encoded.AddBit(rangeMin & 0x80000000);
			rangeMin <<= 1;
			rangeMax <<= 1;
			rangeMax |= 1;
		}
		
		encoded.AddBit(rangeMin & 0x80000000);
		return encoded;
	}
	
	static Array<V> Decode(const BitArray& encoded, const T& model) {
		Array<V> values;
		
		struct CandidateSymbol {
			uint32 Symbol;
			f32 Start;
			f32 End;
		};
		
		Array<CandidateSymbol> candidateSymbols;
		
		auto lGenerateSymbols = [&]()
		{
			candidateSymbols.Reset();
			ArrayView<V> context = ArrayView(values);
			f32 totalProbability = 0.f;
			for (uint32 i=model.GetSymbolCount(); i>0;) {
				--i;
				f32 probability = model.GetSymbolProbability(context, i);
				CHECK(probability >= 0.f);
				CHECK(probability < 100000.f);
				CandidateSymbol& symbol = candidateSymbols.AddRef(CandidateSymbol());
				symbol.Symbol = i;
				symbol.Start = totalProbability;
				totalProbability += probability;
				symbol.End = totalProbability;
			}
			
			f32 terminatorProbability = model.GetTerminatorSymbolProbability(context);
			CHECK(terminatorProbability >= 0.f);
			
			if (terminatorProbability > 0.f) {
				CandidateSymbol& symbol = candidateSymbols.AddRef(CandidateSymbol());
				symbol.Symbol = 0xFFFFFFFF;
				symbol.Start = totalProbability;
				totalProbability += terminatorProbability;
				symbol.End = totalProbability;
			}
			
			for (CandidateSymbol& symbol : candidateSymbols) {
				symbol.Start /= totalProbability;
				symbol.End /= totalProbability;
			}
			
			return candidateSymbols;
		};
		
		candidateSymbols = lGenerateSymbols();
		
		uint32 rangeMin = 0x00000000;
		uint32 rangeMax = 0xFFFFFFFF;
		uint32 code = 0;
		
		uint32 bitPos = 0;
		
		auto lInputBit = [&]() {
			if (UNLIKELY(bitPos >= encoded.GetBitCount())) {
				return false;
			}
			return encoded.GetBit(bitPos++);
		};
		
		for (uint32 i=0; i<32; ++i) {
			code = (code << 1) | (lInputBit() ? 1 : 0);
		}
		
		while (true)
		{
			auto lDecodeSymbol = [&]() {
				uint64 range = uint64(rangeMax) - uint64(rangeMin) + 1;
				f32 scaledValue = ((code - rangeMin + 1) / f32(range));
				
				const CandidateSymbol* symbol = nullptr;
				for (const CandidateSymbol& s : candidateSymbols) {
					if (scaledValue >= s.Start && scaledValue < s.End) {
						symbol = &s;
						break;
					}
				}
				CHECK(!!symbol);
				
				rangeMax = rangeMin + uint32(range * symbol->End) - 1;
				rangeMin = rangeMin + uint32(range * symbol->Start);
				
				while (1)
				{
					if ((rangeMax & 0x80000000) == (rangeMin & 0x80000000))
					{
						// Output the MSB and shift
						rangeMin = (rangeMin << 1) & 0xFFFFFFFF;
						rangeMax = ((rangeMax << 1) | 1) & 0xFFFFFFFF;
					}
					else if ((rangeMin & 0x40000000) && !(rangeMax & 0x40000000))
					{
						// Handle underflow
						code = code ^ 0x40000000;
						rangeMin = (rangeMin << 1) & 0x3FFFFFFF;
						rangeMax = (rangeMax << 1) | 0x80000000;
					}
					else
					{
						break;
					}
					
					rangeMin = rangeMin << 1;
					rangeMax = (rangeMax << 1) | 1;
					code = (code << 1) | lInputBit();
				}
				return symbol->Symbol;
			};
			uint32 symbol = lDecodeSymbol();
			if (symbol == 0xFFFFFFFF) {
				break;
			}
			values.Add(model.ToSymbol(symbol));
			
			candidateSymbols = lGenerateSymbols();
		}
		return values;
	}
};