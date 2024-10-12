// Copyright (c) 2024, Hidde van der Kooij
// SPDX-License-Identifier: BSD-2-Clause

#pragma once

#include "Common/Types.h"
#include "Strings/String.h"
#include "Containers/View/StringView.h"

namespace FilePathStatics {
	// Implement in platform specific code
	char8 GetPlatformPathSeparator();
	bool IsAbsolutePlatform(StringView path);
	
	bool IsDirectory(StringView path);
	bool IsFile(StringView path);
	bool IsAbsolute(StringView path);
	bool IsRelative(StringView path);
	bool NavigatesUp(StringView path);
	
	void FindRootFolder();
	void FindRootFolder(StringView path);
	
	Array<StringView> GetComponents(StringView path);
}

class FilePath {
public:	
	static FilePath* RootFolder;

	FilePath();
	FilePath(const FilePath& copy) = default;
	FilePath(FilePath&& move) = default;
	FilePath(StringView path);
	FilePath(const String& path);
	FilePath(String&& path);
	FilePath& operator=(const FilePath& other) = default;
	FilePath& operator=(FilePath&& other) = default;
	
public:
	const char8* AsCString();
	const char8* AsCString() const;
	StringView AsView() const;
	
	// Path concludes with a slash
	bool IsDirectory() const;
	// Path concludes with a file name
	bool IsFile() const;
	// Path is not relative to the current working directory
	bool IsAbsolute() const;
	// Path is relative to the current working directory
	bool IsRelative() const;
	// True if the path navigates up the directory tree
	bool NavigatesUp() const;
	
	// For filepaths it returns the directory
	// For directories it returns the parent directory
	FilePath GetParent() const;
	
	StringView GetFilename() const;
	StringView GetExtension() const;
	StringView GetDirectory() const;
	
	FilePath operator/(StringView other) const;
	FilePath operator/(FilePath other) const;
protected:
	// Turn a path into a canonical form, ensuring
	// correct path separators, absolute paths start
	// with a slash or drive letter, relative paths start
	// with a dot and a slash or two dots and a slash.
	void Canonicalize();

protected:
	String Path;
};