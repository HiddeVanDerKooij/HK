#pragma

// A hopefully straightfoward disk-backed file

#include "Common/Types.h"
#include "Strings/String.h"
#include "Containers/View/StringView.h"
#include "FilePathBase.h"

class FilePath : public FilePathBase {
public:
	FilePath() = default;
	FilePath(const FilePath& copy) = default;
	FilePath(FilePath&& move) = default;
	FilePath(StringView path)
	{
		Path = path;
		Canonicalize();
	};
	FilePath(const String& path)
	{
		Path = path;
		Canonicalize();
	};
	FilePath(const String&& path)
	{
		Path = path;
		Canonicalize();
	};
	FilePath& operator=(const FilePath& other) = default;
	
protected:
	// Implement in platform specific code
	void Canonicalize();
};

class File {
public:
	File(const File&) = delete;
	File& operator=(const File&) = delete;
	
	// Implement in platform specific code
	File(const FilePath& path);
	~File();
	
	bool Create(bool bReadOnly);
	bool Open(bool bReadOnly);
	void Close();
	bool Delete();
	bool Exists();
	
	bool IsOpen() const;
	bool IsReadable() const;
	bool IsWritable() const;
	
	uint64 Read(uint8* buffer, uint64 size);
	void Write(uint8* buffer, uint64 size);
	void Write(StringView text);
	
	void Flush();
	
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

// Implicit cast from StringView to FilePath