// Copyright (c) 2024, Hidde van der Kooij
// SPDX-License-Identifier: BSD-2-Clause

#include "Common/CompilerMacros.h"

#if PLATFORM == PLATFORM_WINDOWS

#include "File.h"

#include <windows.h>

File::File(const FilePath& path) {
	
	Path = path;
	FileHandlePtr = nullptr;
	FileSize = 0;
	FilePos = 0;
	bOpenRead = false;
	bOpenWrite = false;
	bCheckedExists = false;
	bExists = false;
}

File::~File() {
	if (FileHandlePtr) {
		CloseHandle(FileHandlePtr);
	}
	FileHandlePtr = nullptr;
}

bool File::Create(bool bReadOnly) {
	if (FileHandle) {
		return false;
	}
	
	FileHandlePtr = CreateFileA(Path.AsCString(), bReadOnly ? GENERIC_READ : GENERIC_WRITE, 0, nullptr, bReadOnly ? OPEN_EXISTING : CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (FileHandlePtr == INVALID_HANDLE_VALUE) {
		FileHandlePtr = nullptr;
		bCheckedExists = true;
		bExists = false;
		return false;
	}
	
	LARGE_INTEGER size;
	GetFileSizeEx(FileHandlePtr, &size);
	FileSize = size.QuadPart;
	FilePos = 0;
	
	bOpenRead = bReadOnly;
	bOpenWrite = !bReadOnly;
	bCheckedExists = true;
	bExists = true;
	
	return true;
}

bool File::Open(bool bReadOnly) {
	if (FileHandle) {
		return false;
	}
	
	FileHandlePtr = CreateFileA(Path.AsCString(), bReadOnly ? GENERIC_READ : GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (FileHandlePtr == INVALID_HANDLE_VALUE) {
		FileHandlePtr = nullptr;
		bCheckedExists = true;
		bExists = false;
		return false;
	}
	
	LARGE_INTEGER size;
	GetFileSizeEx(FileHandlePtr, &size);
	FileSize = size.QuadPart;
	FilePos = 0;
	
	bOpenRead = bReadOnly;
	bOpenWrite = !bReadOnly;
	bCheckedExists = true;
	bExists = true;
	
	return true;
}

void File::Close() {
	if (FileHandlePtr) {
		CloseHandle(FileHandlePtr);
	}
	FileHandlePtr = nullptr;
	bOpenRead = false;
	bOpenWrite = false;
}

bool File::Delete() {
	if (FileHandlePtr) {
		Close();
	}
	
	if (DeleteFileA(Path.AsCString())) {
		return true;
	}
	
	return false;
}

bool File::Exists() {
	if (!bCheckedExists) {
		bExists = GetFileAttributesA(Path.AsCString()) != INVALID_FILE_ATTRIBUTES;
		bCheckedExists = true;
	}
	
	return bExists;
}

void File::Seek(uint64 pos) {
	if (!FileHandlePtr) {
		return;
	}
	
	LARGE_INTEGER li;
	li.QuadPart = pos;
	SetFilePointerEx(FileHandlePtr, li, nullptr, FILE_BEGIN);
	FilePos = pos;
}

void File::SeekToEnd() {
	if (!FileHandlePtr) {
		return;
	}
	
	LARGE_INTEGER li;
	li.QuadPart = 0;
	SetFilePointerEx(FileHandlePtr, li, nullptr, FILE_END);
	FilePos = GetSize();
}

bool File::IsOpen() const {
	return FileHandlePtr != nullptr;
}

uint64 File::Read(uint8* buffer, uint64 size) {
	if (!FileHandle && bOpenRead) {
		return 0;
	}
	
	DWORD bytesRead;
	CHECK(ReadFile(FileHandlePtr, buffer, DWORD(size), &bytesRead, nullptr));
	FilePos += bytesRead;
	return bytesRead;
}

void File::Write(const uint8* buffer, uint64 size) {
	if (!FileHandle && bOpenWrite) {
		return;
	}
	
	DWORD bytesWritten;
	WriteFile(FileHandlePtr, buffer, DWORD(size), &bytesWritten, nullptr);
	FilePos += bytesWritten;
}

void File::Write(StringView text) {
	if (!FileHandle && bOpenWrite) {
		return;
	}
	
	DWORD bytesWritten;
	WriteFile(FileHandlePtr, text.Data(), text.Size(), &bytesWritten, nullptr);
	FilePos += bytesWritten;
}

void File::Flush() {
	if (!FileHandlePtr) {
		return;
	}
	
	FlushFileBuffers(FileHandlePtr);
}

bool File::CreateDir() const {
	CHECK(Path.IsDirectory());
	
	return CreateDirectoryA(Path.AsCString(), nullptr);
}

#endif