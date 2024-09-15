#pragma

// A hopefully straightfoward disk-backed file

#include "Common/Types.h"
#include "Containers/View/StringView.h"

class File {
public:
	File(const File&) = delete;
	File& operator=(const File&) = delete;
	
	File(StringView path, bool bReadOnly);
	~File();
	
	bool IsOpen() const;
	
	uint64 Read(uint8* buffer, uint64 size);
	void Write(uint8* buffer, uint64 size);
	void Write(StringView text);
	
	void Flush();
	
protected:
	void* FileHandle;
	uint64 FileSize;
	uint64 FilePos;
};