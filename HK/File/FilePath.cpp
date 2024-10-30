// Copyright (c) 2024, Hidde van der Kooij
// SPDX-License-Identifier: BSD-2-Clause

#include "FilePath.h"
#include "File.h"

#include "Util/Out.h"
#include "Bench/Bench.h"

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
	ScopeBenchmark bench("FindRootFolder"_sv);
	
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

Array<StringView> FilePathStatics::GetComponents(StringView path)
{
	Array<StringView> components(16);
	uint32 currentPos = 0;
	char8 pathSeparator = GetPlatformPathSeparator();
	
	while (true)
	{
		int32 separator = path.LeftFind(pathSeparator, currentPos);
		if (separator == -1)
		{
			components.Add(path.Substring(currentPos, path.Size() - currentPos));
			break;
		}
		else
		{
			components.Add(path.Substring(currentPos, separator-currentPos));
			currentPos = separator+1;
			if (currentPos >= path.Size())
				break;
		}
	}
	return components;
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
		int32 lastSlash = Path.AsView().RightFind(FilePathStatics::GetPlatformPathSeparator());
		
		CHECK(lastSlash != -1);
		String up = Path.AsView();
		up += ".."_sv;
		up += FilePathStatics::GetPlatformPathSeparator();
		FilePath fUp(up);
		return fUp;
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
	
	int32 lastSlash = path.RightFind(FilePathStatics::GetPlatformPathSeparator());
	if (lastSlash == -1)
	{
		return path;
	}
	return path.Substring(uint32(lastSlash) + 1, path.Size() - uint32(lastSlash) - 1);
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

FilePath FilePath::operator+(StringView other) const
{
	return operator+(FilePath(other));
}

FilePath FilePath::operator+(FilePath other) const
{
	String path = Path;
	path += other.Path;
	return FilePath(Move(path));
}

void FilePath::Canonicalize()
{
	ScopeBenchmark bench("Canonicalize"_sv);
	
	// Use proper path separators everywhere
	String newPath = String(Path.Size()+2);
	const char8 targetSeparator = FilePathStatics::GetPlatformPathSeparator();
	const char8 sourceSeparator = (targetSeparator == '/') ? '\\' : '/';
	
	for (uint32 i=0; i<Path.Size(); ++i) {
		ScopeBenchmark bench("Separator"_sv);
		char8 c = Path.At(i);
		if (c == sourceSeparator) {
			c = targetSeparator;
		}
		newPath += c;
	}
	
	// Now we need to ensure it's absolute, or relative
	/*
	bool bAbsolute = FilePathStatics::IsAbsolute(newPath.AsView());
	if (!bAbsolute) {
		if (!FilePathStatics::NavigatesUp(newPath.AsView())) {
			newPath = String("."_sv) + targetSeparator + newPath;
		}
	}
	*/
	
	// If we have any parent directory traversals we clean them up
	// Also we clean up any current directory reference except
	// the first one.
	bool isFile = FilePathStatics::IsFile(newPath.AsView());
	Array<StringView> components;
	if (FilePathStatics::IsRelative(newPath.AsView()))
	{
		components.Add("."_sv);
	}
	components.AddRange(ArrayView<StringView>(FilePathStatics::GetComponents(newPath.AsView())));
	
	for (uint32 i=components.Num()-1; i>0 && i < components.Num(); --i)
	{
		ScopeBenchmark bench("Component iteration"_sv);
		
		StringView current = components[i];
		
		if (current == ".."_sv)
		{
			StringView prev = components[i-1];
			if (prev != ".."_sv && prev != "."_sv)
			{
				components.RemoveAt(i);
				components.RemoveAt(i-1);
				--i;
				// TODO (HvdK): RemoveAtN(i-1, 2)
			}
		}
		else if (current == "."_sv)
		{
			components.RemoveAt(i);
		}
	}
	
	String cleanedPath(newPath.Size());
	
	for (uint32 i=0; i<components.Num(); ++i)
	{
		auto& component = components[i];
		cleanedPath += component;
		if (!isFile || (i+1) < components.Num())
			cleanedPath += targetSeparator;
	}
	
	cleanedPath.AsCString();
	Path = Move(cleanedPath);
}
