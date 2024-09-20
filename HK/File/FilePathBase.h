#pragma once

#include "Common/Types.h"
#include "Strings/String.h"
#include "Containers/View/StringView.h"

class FilePathBase {
protected:
	void CanonicalizeForwardSlash();
	void CanonicalizeBackwardSlash();
public:
	const char8* AsCString() const;
	StringView AsView() const;
protected:
	String Path;
};