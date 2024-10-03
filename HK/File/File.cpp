#include "File.h"

#include "Common/CompilerMacros.h"

bool File::IsReadable() const {
	return bOpenRead;
}

bool File::IsWritable() const {
	return bOpenWrite;
}

uint64 File::GetSize() const {
	CHECK(IsOpen());
	return FileSize;
}