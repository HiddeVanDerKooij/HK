#pragma once

#include "Common/Types.h"
#include "Strings/String.h"

class Out {
public:
	static void Write(StringView str);
	static void Write(const String& str);
	static void WriteLine(StringView str);
	static void WriteLine(const String& str);


private:
	static void WriteImpl(StringView str, bool bNewLine);
};