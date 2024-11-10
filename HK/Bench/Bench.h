// Copyright (c) 2023-2024, Hidde van der Kooij
// SPDX-License-Identifier: BSD-2-Clause

#pragma once

#include "Common/Types.h"
#include "Containers/View/StringView.h"

class ScopeBenchmark
{
public:
	ScopeBenchmark(StringView name);
	~ScopeBenchmark();
protected:
	uint32 id;
};

struct BenchmarkEntryRun
{
	StringView Name;
	uint64 Start;
	uint64 End;
};

class Benchmark
{
public:
	static uint32 StartRun(StringView name);
	static void StopRun(uint32 id);
	static void Print();
};