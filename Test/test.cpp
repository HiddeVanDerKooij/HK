#include <iostream>

#include "Common/Types.h"
#include "Common/Math.h"
#include "Containers/View/StringView.h"
#include "Containers/View/ArrayView.h"
#include "Compression/ArithmeticCoding.h"

int main()
{
	std::cout << "Test" << std::endl;
	
	//StringView input = "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAB"_sv;
	static StringView input = "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBCADABDKSUEEiieeeehsksJeJeJEJEeEej23122239123WErjeAHHHHEHHHEHEHHHHEHEHEHEEEHEHE22222222222eeekkkhjsjss2229cvvvywwy2n3nrjrjj"_sv;
	
	struct symbol {
		f32 count = 0;
	};
	
	Array<symbol> symbols;
	for (int32 i=0; i<127; ++i)
		symbols.AddDefaulted();
	
	const char8* scanner = input.Data();
	while (!!*scanner)
	{
		symbols[*scanner++].count += 1.f;
	}
	
	struct Model {
		uint32 GetSymbolCount() const {
			return Symbols.Num();
		};
		f32 GetSymbolProbability(ArrayView<char8> encoded, uint32 symbol) const {
			return Symbols[symbol].count;
		};
		f32 GetTerminatorSymbolProbability(ArrayView<char8> encoded) const {
			//if (encoded.Size() + 5 < input.Size())
			//	return 0.f;
			return 1.f;
		};
		uint32 ToIndex(char8 c) const {
			return uint32(c);
		}
		char8 ToSymbol(uint32 index) const {
			return char8(index);
		}
		Array<symbol> Symbols;
	} model;
	
	model.Symbols = symbols;
	
	ArrayView<char8> inputArrayView = ArrayView(input.Data(), input.Size());
	BitArray encoded = ArithmeticCoding<Model, char8>::Encode(inputArrayView, model);
	
	std::cout << "Encoded size: " << encoded.GetBitCount() << std::endl;
	for (int32 i=0; i<encoded.GetBitCount(); ++i) {
		std::cout << (encoded.GetBit(i) ? "1" : "0");
	}
	
	std::cout << std::endl;
	
	Array<char8> decoded = ArithmeticCoding<Model, char8>::Decode(encoded, model);
	
	std::cout << "Decoded size: " << decoded.Num() << std::endl;
	for (int32 i=0; i<decoded.Num(); ++i) {
		std::cout << decoded[i];
	}	
	
	return 0;
}
