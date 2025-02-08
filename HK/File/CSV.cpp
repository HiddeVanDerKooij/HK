// Copyright (c) 2025, Hidde van der Kooij
// SPDX-License-Identifier: BSD-2-Clause

#include "CSV.h"
#include "Bench/Bench.h"

CSV::CSV()
{}

CSV::~CSV()
{}

CSV CSV::Create(StringView contents)
{
	CSV result;
	
	Array<StringView> lines = contents.Split('\n');
	if (lines.Num() == 0) return result;
	
	Array<StringView> headers = lines[0].Split(',');
	for (uint32 i=0; i<headers.Num(); ++i)
	{
		result.Headers.Add({headers[i], i});
	}
	
	for (uint32 i=1; i<lines.Num(); ++i)
	{
		Array<StringView> columns = lines[i].Split(',');
		if (columns.Num() != headers.Num()) continue;
		result.Rows.Add({lines[i]});
	}
	
	return result;
}