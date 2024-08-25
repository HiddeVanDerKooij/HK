#include <iostream>

#include "Out.h"


void Out::Write(StringView str) {
	WriteImpl(str, false);
}

void Out::Write(const String& str) {
	WriteImpl(str.AsView(), false);
}

void Out::WriteLine(StringView str) {
	WriteImpl(str, true);
}

void Out::WriteLine(const String& str) {
	WriteImpl(str.AsView(), true);
}

void Out::WriteImpl(StringView str, bool bNewLine) {

	String output = String();

	if (bNewLine) {
		output = String::Format("{}\n", str);
	}
	else {
		output = String::Format("{}", str);
	}
	std::cout << *output;
}