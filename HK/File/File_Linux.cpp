// Copyright (c) 2024, Hidde van der Kooij
// SPDX-License-Identifier: BSD-2-Clause

#include "Common/CompilerMacros.h"

#if PLATFORM == PLATFORM_LINUX

#include "File.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

File::File(const FilePath& path) {
	
	Path = path;
	FileHandle = 0;
	FileSize = 0;
	FilePos = 0;
	bOpenRead = false;
	bOpenWrite = false;
	bCheckedExists = false;
	bExists = false;
}

File::~File() {
	if (!!FileHandle) {
		close(FileHandle);
	}
	FileHandle = 0;
}

bool File::Create(bool bReadOnly) {
	if (!!FileHandle) {
		return false;
	}
	
	FileHandle = open(Path.AsCString(), bReadOnly ? O_RDONLY : O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
	if (FileHandle == -1) {
		FileHandle = 0;
		bCheckedExists = true;
		bExists = false;
		return false;
	}
	
	struct stat st;
	fstat(FileHandle, &st);
	FileSize = st.st_size;
	FilePos = 0;
	
	bOpenRead = bReadOnly;
	bOpenWrite = !bReadOnly;
	bCheckedExists = true;
	bExists = true;
	return true;
}

bool File::Open(bool bReadOnly) {
	if (!!FileHandle) {
		return false;
	}
	
	FileHandle = open(Path.AsCString(), bReadOnly ? O_RDONLY : O_WRONLY, S_IRUSR | S_IWUSR);
	if (FileHandle == -1) {
		FileHandle = 0;
		bCheckedExists = true;
		bExists = false;
		return false;
	}
	
	struct stat st;
	fstat(FileHandle, &st);
	FileSize = st.st_size;
	FilePos = 0;
	
	bOpenRead = bReadOnly;
	bOpenWrite = !bReadOnly;
	bCheckedExists = true;
	bExists = true;
	return true;
}

void File::Close() {
	if (!!FileHandle) {
		close(FileHandle);
	}
	FileHandle = 0;
	bOpenRead = false;
	bOpenWrite = false;
}

bool File::Delete() {
	if (!!FileHandle) {
		Close();
	}
	
	if (unlink(Path.AsCString()) == 0) {
		return true;
	}
	
	return false;
}

bool File::Exists() {
	if (!bCheckedExists) {
		struct stat st;
		bExists = stat(Path.AsCString(), &st) == 0;
		bCheckedExists = true;
	}
	
	return bExists;
}

void File::Seek(uint64 pos) {
	if (!FileHandle) {
		return;
	}
	
	lseek(FileHandle, pos, SEEK_SET);
	FilePos = pos;
}

void File::SeekToEnd() {
	if (!FileHandle) {
		return;
	}
	
	FilePos = lseek(FileHandle, 0, SEEK_END);
}

bool File::IsOpen() const {
	return !!FileHandle;
}

uint64 File::Read(uint8* buffer, uint64 size) {
	if (!FileHandle && bOpenRead) {
		return 0;
	}
	
	uint64 bytesRead = read(FileHandle, buffer, size);
	FilePos += bytesRead;
	return bytesRead;
}

void File::Write(const uint8* buffer, uint64 size) {
	if (!FileHandle && bOpenWrite) {
		return;
	}
	
	write(FileHandle, buffer, size);
	FilePos += size;
}

void File::Write(StringView text) {
	if (!FileHandle && bOpenWrite) {
		return;
	}
	
	write(FileHandle, text.Data(), text.Size());
	FilePos += text.Size();
}

void File::Flush() {
	fsync(FileHandle);
}

bool File::CreateDirectory() const {
	CHECK(Path.IsDirectory());
	
	if (mkdir(Path.AsCString(), S_IRWXU) == 0) {
		return true;
	}
	return false;
}

#endif