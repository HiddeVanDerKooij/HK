// Copyright (c) 2023-2024, Hidde van der Kooij
// SPDX-License-Identifier: BSD-2-Clause

#include "Bench.h"

#include "Util/Platform.h"
#include "Util/Out.h"

ScopeBenchmark::ScopeBenchmark(StringView name)
{
	id = Benchmark::StartRun(name);
}

ScopeBenchmark::~ScopeBenchmark()
{
	Benchmark::StopRun(id);
}

static BenchmarkEntryRun ActiveRun[256] = {};
static uint32 ActiveRunCount = 0;

uint32 Benchmark::StartRun(StringView name)
{
	uint32 id = ActiveRunCount++;
	ActiveRun[id].Name = name;
	ActiveRun[id].Start = Platform::GetTicks();
	ActiveRun[id].End = 0;
	return id;
}

void Benchmark::StopRun(uint32 id)
{
	ActiveRun[id].End = Platform::GetTicks();
	ActiveRunCount = id;
}

void Benchmark::Print()
{
	for (uint32 i=0; i<ActiveRunCount; ++i)
	{
		Out::WriteLine("{}: {}"_sv, ActiveRun[i].Name, ActiveRun[i].Start);
	}
}