// Copyright (c) 2024, Hidde van der Kooij
// SPDX-License-Identifier: BSD-2-Clause

#include "Common/CompilerMacros.h"

#if PLATFORM == PLATFORM_WINDOWS

#include "File.h"

#include <windows.h>

File::File(const FilePath& path) {
	
	Path = path;
	FileHandle = nullptr;
	FileSize = 0;
	FilePos = 0;
	bOpenRead = false;
	bOpenWrite = false;
	bCheckedExists = false;
	bExists = false;
}

File::~File() {
	if (FileHandle) {
		CloseHandle(FileHandle);
	}
	FileHandle = nullptr;
}

bool File::Create(bool bReadOnly) {
	if (FileHandle) {
		return false;
	}
	
	FileHandle = CreateFileA(path.AsCString(), bReadOnly ? GENERIC_READ : GENERIC_WRITE, 0, nullptr, bReadOnly ? OPEN_EXISTING : CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (FileHandle == INVALID_HANDLE_VALUE) {
		FileHandle = nullptr;
		bCheckedExists = true;
		bExists = false;
	}
	
	LARGE_INTEGER size;
	GetFileSizeEx(FileHandle, &size);
	FileSize = size.QuadPart;
	FilePos = 0;
	
	bOpenRead = bReadOnly;
	bOpenWrite = !bReadOnly;
	bCheckedExists = true;
	bExists = true;
}

bool File::Open(bool bReadOnly) {
	if (FileHandle) {
		return false;
	}
	
	FileHandle = CreateFileA(path.Data(), bReadOnly ? GENERIC_READ : GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (FileHandle == INVALID_HANDLE_VALUE) {
		FileHandle = nullptr;
		bCheckedExists = true;
		bExists = false;
	}
	
	LARGE_INTEGER size;
	GetFileSizeEx(FileHandle, &size);
	FileSize = size.QuadPart;
	FilePos = 0;
	
	bOpenRead = bReadOnly;
	bOpenWrite = !bReadOnly;
	bCheckedExists = true;
	bExists = true;
}

void File::Close() {
	if (FileHandle) {
		CloseHandle(FileHandle);
	}
	FileHandle = nullptr;
	bOpenRead = false;
	bOpenWrite = false;
}

bool File::Delete() {
	if (FileHandle) {
		Close();
	}
	
	if (DeleteFileA(Path.Data())) {
		return true;
	}
	
	return false;
}

bool File::Exists() const {
	if (!bCheckedExists) {
		bExists = GetFileAttributesA(Path.Data()) != INVALID_FILE_ATTRIBUTES;
		bCheckedExists = true;
	}
	
	return bExists;
}

bool File::IsOpen() const {
	return FileHandle != nullptr;
}

bool File::IsReadable() const {
	return bOpenRead;
}

bool File::IsWritable() const {
	return bOpenWrite;
}

uint64 File::Read(uint8* buffer, uint64 size) {
	if (!FileHandle && bOpenRead) {
		return 0;
	}
	
	DWORD bytesRead;
	ReadFile(FileHandle, buffer, DWORD(size), &bytesRead, nullptr);
	FilePos += bytesRead;
	return bytesRead;
}

void File::Write(uint8* buffer, uint64 size) {
	if (!FileHandle && bOpenWrite) {
		return;
	}
	
	DWORD bytesWritten;
	WriteFile(FileHandle, buffer, DWORD(size), &bytesWritten, nullptr);
	FilePos += bytesWritten;
}

void File::Write(StringView text) {
	if (!FileHandle && bOpenWrite) {
		return;
	}
	
	DWORD bytesWritten;
	WriteFile(FileHandle, text.Data(), text.Size(), &bytesWritten, nullptr);
	FilePos += bytesWritten;
}

void File::Flush() {
	if (!FileHandle) {
		return;
	}
	
	FlushFileBuffers(FileHandle);
}

#endif