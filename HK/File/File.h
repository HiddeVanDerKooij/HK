// Copyright (c) 2024, Hidde van der Kooij
// SPDX-License-Identifier: BSD-2-Clause

#pragma

// A hopefully straightfoward disk-backed file

#include "Common/Types.h"
#include "Strings/String.h"
#include "Containers/View/StringView.h"
#include "FilePath.h"

class File {
public:
	File(const File&) = delete;
	File& operator=(const File&) = delete;
	
	File(const FilePath& path);
	~File();
	
	bool Create(bool bReadOnly);
	bool Open(bool bReadOnly);
	void Close();
	bool Delete();
	bool Exists();
	uint64 GetSize() const;
	
	bool IsOpen() const;
	bool IsReadable() const;
	bool IsWritable() const;
	
	uint64 Read(uint8* buffer, uint64 size);
	void Write(const uint8* buffer, uint64 size);
	void Write(StringView text);
	
	void Flush();
	
	// Create all directories in the path
	bool CreateDirectory() const;
	bool CreateDirectories() const;
	
protected:
	FilePath Path;
	union {
		void* FileHandlePtr;
		int32 FileHandle;
	};
	uint64 FileSize;
	uint64 FilePos;
	bool bOpenRead : 1;
	bool bOpenWrite : 1;
	bool bCheckedExists : 1;
	bool bExists : 1;
};