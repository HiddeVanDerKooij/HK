// Copyright (c) 2025, Hidde van der Kooij
// SPDX-License-Identifier: BSD-2-Clause

#pragma once

#include "Common/Types.h"
#include "Containers/Array.h"
#include "Containers/HashMap.h"
#include "Containers/View/StringView.h"
#include "Strings/String.h"

#define CSV_ADD_PROPERTY(_csv, _column, _member) _csv.AddProperty(_column, GCSVReader::EPropertyMissingRules::Error, ADDRESS_OF(decltype(_csv)::PropertyType, _member))
#define CSV_ADD_PROPERTY_EXT(_csv, _column, _member, _missing) _csv.AddProperty(_column, _missing, ADDRESS_OF(decltype(_csv)::PropertyType, _member))

class GCSVReader
{
public:
	enum class EPropertyMissingRules
	{
		ClassDefault,
		Error,
		CopyPrevious
	};
	
protected:
	enum class EPropertyType
	{
		Int32,
		UInt32,
		Float,
		StringView,
	};
	
	void AddPropertyInternal(StringView column, const void* address, EPropertyType type, EPropertyMissingRules missingRule);
	
	// Returns false if the header is invalid
	// Stores the error in a global variable you can get with
	// GetError()
	// Mutates csv to point to the first row
	bool ParseHeader(StringView& csv, Array<int32>& outPropertyIndices) const;
	bool ParseRow(StringView& csv, uint32 rowCount, void* data, const void* classDefault, const Array<int32>& propertyIndices, bool& bError) const;
	
public:
	static StringView GetError();
	
private:
	struct ColumnName
	{
		StringView Name;
		
		void Hash(Hasher<uint32>& hasher) const;
		bool operator==(const ColumnName& other) const;
	};

	struct Property
	{
		StringView Name;
		EPropertyType Type;
		const void* Address;
		EPropertyMissingRules MissingRule;
	};
	
	HashMap<ColumnName, uint32> ColumnNameToPropertyIndex;
	Array<Property> Properties;
};

template<typename T>
class CSVReader : public GCSVReader
{
public:
	typedef T PropertyType;
	
	void AddProperty(StringView column, EPropertyMissingRules missingRule, const int32* address);
	void AddProperty(StringView column, EPropertyMissingRules missingRule, const uint32* address);
	void AddProperty(StringView column, EPropertyMissingRules missingRule, const f32* address);
	void AddProperty(StringView column, EPropertyMissingRules missingRule, const StringView* address);
	
	Array<T> GetRows(StringView csv, uint32 skipRows) const;
};

template<typename T>
void CSVReader<T>::AddProperty(StringView column, EPropertyMissingRules missingRule, const int32* address)
{
	AddPropertyInternal(column, address, EPropertyType::Int32, missingRule);
}

template<typename T>
void CSVReader<T>::AddProperty(StringView column, EPropertyMissingRules missingRule, const uint32* address)
{
	AddPropertyInternal(column, address, EPropertyType::UInt32, missingRule);
}

template<typename T>
void CSVReader<T>::AddProperty(StringView column, EPropertyMissingRules missingRule, const f32* address)
{
	AddPropertyInternal(column, address, EPropertyType::Float, missingRule);
}

template<typename T>
void CSVReader<T>::AddProperty(StringView column, EPropertyMissingRules missingRule, const StringView* address)
{
	AddPropertyInternal(column, address, EPropertyType::StringView, missingRule);
}

template<typename T>
Array<T> CSVReader<T>::GetRows(StringView csv, uint32 skipRows) const
{
	// ScopeBenchmark _f("CSVReader<T>::GetRows"_sv);
	
	Array<int32> propertyIndices;
	if (!ParseHeader(csv, propertyIndices))
	{
		return Array<T>();
	}
	
	while (skipRows > 0)
	{
		--skipRows;
		
		int32 rowEnd = csv.LeftFind('\n');
		if (rowEnd == -1)
		{
			return Array<T>();
		}
		csv = csv.ChopLeft(rowEnd + 1);
	}
	
	Array<T> result;
	
	const T classDefault = T();
	T row = classDefault;
	bool bError = false;
	while (ParseRow(csv, result.Num(), &row, &classDefault, propertyIndices, bError))
	{
		if (bError)
		{
			return Array<T>();
		}
		result.Add(row);
	}
	
	return result;
}