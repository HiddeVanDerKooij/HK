#include "File.h"

#include "Common/CompilerMacros.h"
#include "Util/Out.h"

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
	
	return File(path).CreateDir();
}

bool File::CreateDirectories() const
{
	return CreateDirectoriesRecursive(Path);
}

void File::PrintState() const
{
	Out::WriteLine("File: {}"_sv, Path.AsView());
	Out::WriteLine("Size: {}"_sv, FileSize);
	Out::WriteLine("Pos: {}"_sv, FilePos);
	Out::WriteLine("OpenRead: {}"_sv, bOpenRead);
	Out::WriteLine("OpenWrite: {}"_sv, bOpenWrite);
	Out::WriteLine("Exists: {}"_sv, bExists);
	Out::WriteLine("FileHandle: {}, {}"_sv, FileHandle, FileHandlePtr);
}