#include "Common/CompilerMacros.h"
#include "File.h"

#if PLATFORM == PLATFORM_WINDOWS

#include <windows.h>

File::File(StringView path, bool bReadOnly) {
	FileHandle = CreateFileA(path.Data(), bReadOnly ? GENERIC_READ : GENERIC_WRITE, 0, nullptr, bReadOnly ? OPEN_EXISTING : CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (FileHandle == INVALID_HANDLE_VALUE) {
		FileHandle = nullptr;
	}
	
	LARGE_INTEGER size;
	GetFileSizeEx(FileHandle, &size);
	FileSize = size.QuadPart;
	FilePos = 0;
}

File::~File() {
	if (FileHandle) {
		CloseHandle(FileHandle);
	}
	FileHandle = nullptr;
}

bool File::IsOpen() const {
	return FileHandle != nullptr;
}

uint64 File::Read(uint8* buffer, uint64 size) {
	if (!FileHandle) {
		return 0;
	}
	
	DWORD bytesRead;
	ReadFile(FileHandle, buffer, DWORD(size), &bytesRead, nullptr);
	FilePos += bytesRead;
	return bytesRead;
}

void File::Write(uint8* buffer, uint64 size) {
	if (!FileHandle) {
		return;
	}
	
	DWORD bytesWritten;
	WriteFile(FileHandle, buffer, DWORD(size), &bytesWritten, nullptr);
	FilePos += bytesWritten;
}

void File::Write(StringView text) {
	if (!FileHandle) {
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