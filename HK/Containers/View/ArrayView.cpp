// Copyright (c) 2023-2024, Hidde van der Kooij
// SPDX-License-Identifier: BSD-2-Clause

#include "Common/CompilerMacros.h"

#include "Containers/View/ArrayView.h"

GArrayView::GArrayView(const void* data, uint32 size)
	: ArrayData(data), ArraySize(size)
{
	CHECK(data == nullptr && size == 0 || data != nullptr && size > 0);
	CHECK(size < (1 << 30));
}

uint32 GArrayView::Size() const
{
	return ArraySize;
}

void GArrayView::Reset()
{
	ArrayData = nullptr;
	ArraySize = 0;
}

void GArrayView::CheckIndex(uint32 index) const
{
	CHECK(index < ArraySize);
}