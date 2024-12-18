// Copyright (c) 2023-2024, Hidde van der Kooij
// SPDX-License-Identifier: BSD-2-Clause

#pragma once

#include "Types.h"
#include "Containers/View/StringView.h"
#include "Strings/String.h"

/*
template<typename... Params>
class VariadicPrintParameters {
	VariadicPrintParameters(Params... params) : Params(params...) {}
};

void PrintImpl(StringView format, const VariadicPrintParameters& params);

template<typename... Params>
void Print(StringView format, Params const&... params) {
	PrintImpl(format, VariadicPrintParameters(params...));
}
*/