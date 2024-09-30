// Copyright (c) 2023-2024, Hidde van der Kooij
// SPDX-License-Identifier: BSD-2-Clause

#pragma once

#include "Strings/String.h"
#include "DualType.h"

#define RETURN_ERROR(error) if (error.IsError()) return error;
#define ERROR_WRAP(error) {auto __error = error; if (__error.IsError()) return __error; }
#define CHECK_ERROR(error) CHECK(!error.IsError())

// TODO (HvdK): This class sucks because of premature optimization.
// As we use a dual type we cannot use complex (de)structors.
// If we just straight-up stored Strings instead of a StringView
// as a non-dualtype we could have so much accessibility in writing
// code.
// Now our ASTContext needs to call SignalError when it needs to use
// String::Format, and Error when it's just a simple view.
// Yes it's performant, but we don't need this.

// A shorthand for having less templated bloat
class Error {
public:
	Error(StringView error) : ErrorString(error) {}
	StringView ErrorString;
};

// A type that is either an error, returned as a
// StringView or a value of type T.
template<typename T>
class ErrorOr : protected DualType<StringView, T> {
public:
	// Implicitly convert from the valuetype to ErrorOr as value
	ErrorOr(const T& value) : DualType<StringView, T>() {
		this->Value2 = value;
		this->state = 2;
	}
	// Implicitly convert from an Error to ErrorOr as error
	ErrorOr(const Error& error) : DualType<StringView, T>() {
		this->Value1 = error.ErrorString;
		this->state = 1;
	}

	// Copy constructor
	ErrorOr(const ErrorOr<T>& other) : DualType<StringView, T>(other) {}

	// Implicitly convert for a different ErrorOr to this ErrorOr
	template <typename U>
	ErrorOr(const ErrorOr<U>& other) : DualType<StringView, T>() {
		if (other.IsError()) {
			this->Value1 = other.GetError();
			this->state = 1;
		} else {
			// TODO (HvdK): Awkward, but we default construct when we don't have a value
			this->Value2 = T();
			this->state = 2;
		}
	}
protected:
	// No blank constructor
	ErrorOr() : DualType<StringView, T>() {};

public:
	bool IsError() const { return this->Is1(); }
	bool IsValue() const { return this->Is2(); }
	StringView GetError() const { return this->Get1(); }
	T& GetValue() { return this->Get2(); }
	const T& GetValue() const { return this->Get2(); }
	T& operator*() { return this->GetValue(); }
	const T& operator*() const { return this->GetValue(); }
};