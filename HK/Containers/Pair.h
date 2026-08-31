// Copyright (c) 2026, Hidde van der Kooij
// SPDX-License-Identifier: BSD-2-Clause

#pragma once

template<typename A, typename B>
struct Pair {
public:
    union {
        struct {A Key; B Value; };
        struct {A First; B Second; };
    };

    Pair() = default;
    Pair(const A& key, const B& value) {
        First = key; Second = value;
    };
    Pair(A&& key, B&& value) {
        First = key; Second = value;
    };
};