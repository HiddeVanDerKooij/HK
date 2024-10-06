// Copyright (c) 2024, Hidde van der Kooij
// SPDX-License-Identifier: BSD-2-Clause

#include "FilePath.h"
#include "File.h"

FilePath* FilePath::RootFolder = nullptr;

bool FilePathStatics::IsDirectory(StringView path)
{
	return path.LastChar() == GetPlatformPathSeparator();
}

bool FilePathStatics::IsFile(StringView path)
{
	return !IsDirectory(path);
}

bool FilePathStatics::IsAbsolute(StringView path)
{
	if (IsAbsolutePlatform(path))
	{
		return true;
	}
	return path.FirstChar() == GetPlatformPathSeparator();
}

bool FilePathStatics::IsRelative(StringView path)
{
	return !IsAbsolute(path);
}

bool FilePathStatics::NavigatesUp(StringView path)
{
	if (path.Size() < 3)
	{
		return false;
	}
	return path.StartsWith(".."_sv) && path.At(2) == GetPlatformPathSeparator();
}

void FilePathStatics::FindRootFolder()
{
	FindRootFolder("./"_sv);
}

void FilePathStatics::FindRootFolder(StringView path)
{
	static FilePath rf = FilePath(path);
	FilePath::RootFolder = &rf;
	
	if (!FilePath::RootFolder->IsDirectory())
	{
		*FilePath::RootFolder = FilePath::RootFolder->GetParent();
	}
	
	//*FilePath::RootFolder = FilePath("."_sv);
	FilePath contender = *FilePath::RootFolder;
	for (int32 i=0; i<8; ++i)
	{
		if (File(contender/"rootfolder"_sv).Exists())
		{
			*FilePath::RootFolder = contender;
			break;
		}
		contender = contender.GetParent();
	}
}

FilePath::FilePath()
{
	Path = "./"_sv;
	Canonicalize();
}

FilePath::FilePath(StringView path)
{
	Path = path;
	Canonicalize();
}

FilePath::FilePath(const String& path)
{
	Path = path;
	Canonicalize();
}

FilePath::FilePath(String&& path)
{
	Path = Move(path);
	Canonicalize();
}

const char8* FilePath::AsCString()
{
	return Path.AsCString();
}

const char8* FilePath::AsCString() const
{
	return Path.AsCString();
}

StringView FilePath::AsView() const
{
	return Path.AsView();
}

bool FilePath::IsDirectory() const
{
	return FilePathStatics::IsDirectory(Path.AsView());
}

bool FilePath::IsFile() const
{
	return FilePathStatics::IsFile(Path.AsView());
}

bool FilePath::IsAbsolute() const
{
	return FilePathStatics::IsAbsolute(Path.AsView());
}

bool FilePath::IsRelative() const
{
	return FilePathStatics::IsRelative(Path.AsView());
}

bool FilePath::NavigatesUp() const
{
	return FilePathStatics::NavigatesUp(Path.AsView());
}

FilePath FilePath::GetParent() const
{
	if (IsDirectory())
	{
		StringView path = Path.AsView().ChopRight(1);
		int32 lastSlash = path.RightFind(FilePathStatics::GetPlatformPathSeparator());
		if (lastSlash == -1)
		{
			String up = ".."_sv;
			up += FilePathStatics::GetPlatformPathSeparator();
			return FilePath(up);
		}
		if (NavigatesUp())
		{
			
		}
		return FilePath(path.Substring(0, lastSlash + 1));
	}
	else
	{
		return FilePath(GetDirectory());
	}
}

StringView FilePath::GetFilename() const
{
	CHECK(!IsDirectory());
	
	StringView path = Path.AsView();
	
	uint32 lastSlash = path.RightFind(FilePathStatics::GetPlatformPathSeparator());
	if (lastSlash == -1)
	{
		return path;
	}
	return path.Substring(lastSlash + 1, path.Size() - lastSlash - 1);
}

StringView FilePath::GetExtension() const
{
	CHECK(!IsDirectory());
	
	StringView filename = GetFilename();
	int32 result = filename.LeftFind('.');
	if (result == -1)
	{
		return StringView();
	}
	return filename.Substring(result + 1, filename.Size() - result - 1);
}

StringView FilePath::GetDirectory() const
{
	CHECK(!IsDirectory());
	
	StringView path = Path.AsView();
	
	int32 lastSlash = path.RightFind(FilePathStatics::GetPlatformPathSeparator());
	if (lastSlash == -1)
	{
		CHECK(IsRelative());
		static String currentDirectory;
		if (currentDirectory.Size() == 0)
		{
			currentDirectory += '.';
			currentDirectory += FilePathStatics::GetPlatformPathSeparator();
		}
		return StringView();
	}
	return path.Substring(0, lastSlash + 1);
}

FilePath FilePath::operator/(StringView other) const
{
	return operator/(FilePath(other));
}

FilePath FilePath::operator/(FilePath other) const
{
	CHECK(!IsFile());
	CHECK(!other.IsAbsolute());
	
	String path = Path;
	path += other.Path;
	return FilePath(Move(path));
}

void FilePath::Canonicalize()
{
	String newPath = String(Path.Size()+2);
	const char8 targetSeparator = FilePathStatics::GetPlatformPathSeparator();
	const char8 sourceSeparator = (targetSeparator == '/') ? '\\' : '/';
	
	for (uint32 i=0; i<Path.Size(); ++i) {
		char8 c = Path.At(i);
		if (c == sourceSeparator) {
			c = targetSeparator;
		}
		newPath += c;
	}
	
	// Now we need to ensure it's absolute, or relative
	bool bAbsolute = FilePathStatics::IsAbsolute(newPath.AsView());
	if (!bAbsolute) {
		if (!FilePathStatics::NavigatesUp(newPath.AsView())) {
			newPath = String("."_sv) + targetSeparator + newPath;
		}
	}
	
	newPath.AsCString();
	Path = Move(newPath);
}
