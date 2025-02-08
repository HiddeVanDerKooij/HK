// Copyright (c) 2025, Hidde van der Kooij
// SPDX-License-Identifier: BSD-2-Clause

#pragma once

#include "Common/Types.h"
#include "Containers/Array.h"
#include "Containers/View/StringView.h"

class CSV
{
protected:
	CSV();
	~CSV();
	
public:
	static CSV Create(StringView contents);
	
	bool GetColumnIndex(StringView title, uint32& outIndex) const;
	StringView At(uint32 row, uint32 column) const;
	
protected:
	struct Header
	{
		StringView Title;
		uint32 Index;
	};
	
	struct Row
	{
		StringView Contents;
	};
	
	Array<Header> Headers;
	Array<Row> Rows;
};