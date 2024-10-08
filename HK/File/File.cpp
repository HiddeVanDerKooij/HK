#include "File.h"

#include "Common/CompilerMacros.h"

bool File::IsReadable() const
{
	return bOpenRead;
}

bool File::IsWritable() const
{
	return bOpenWrite;
}

uint64 File::GetSize() const
{
	CHECK(IsOpen());
	return FileSize;
}

uint64 File::GetPos() const
{
	CHECK(IsOpen());
	return FilePos;
}	

static bool CreateDirectoriesRecursive(FilePath path)
{
	if (!path.IsDirectory()) {
		path = path.GetParent();
	}
	
	if (File(path).Exists()) {
		return true;
	}
	if (!CreateDirectoriesRecursive(path.GetParent())) {
		return false;
	}
	
	return File(path).CreateDirectory();
}

bool File::CreateDirectories() const
{
	return CreateDirectoriesRecursive(Path);
}