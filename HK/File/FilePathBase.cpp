#include "FilePathBase.h"

void FilePathBase::CanonicalizeForwardSlash()
{
	String newPath  = ""_sv;
	
	for (uint32 i=0; i<Path.Size(); ++i) {
		char8 c = Path.At(i);
		if (c == '\\') {
			c = '/';
		}
		newPath += c;
	}
	newPath.AsCString();
	Path = Move(newPath);
}

void FilePathBase::CanonicalizeBackwardSlash()
{
	String newPath  = ""_sv;
	
	for (uint32 i=0; i<Path.Size(); ++i) {
		char8 c = Path.At(i);
		if (c == '/') {
			c = '\\';
		}
		newPath += c;
	}
	newPath.AsCString();
	Path = Move(newPath);
}

const char8* FilePathBase::AsCString() const
{
	return Path.AsCString();
}

StringView FilePathBase::AsView() const
{
	return Path.AsView();
}