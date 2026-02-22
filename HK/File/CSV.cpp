// Copyright (c) 2025, Hidde van der Kooij
// SPDX-License-Identifier: BSD-2-Clause

#include "Common/Types.h"
#include "CSV.h"
#include "Bench/Bench.h"
#include "Common/StringUtil.h"

static String LastError;

void GCSVReader::AddPropertyInternal(StringView columnname,
	const void* address, EPropertyType type, EPropertyMissingRules missingRule)
{
	ScopeBenchmark _f("GCSV::AddPropertyInternal"_sv);
	
	ColumnName column;
	column.Name = columnname;
	
	uint32* exists = ColumnNameToPropertyIndex.Find(column);
	CHECK(exists == nullptr);
	
	ColumnNameToPropertyIndex.Add(column) = Properties.Num();
	
	Property property;
	property.Name = columnname;
	property.Type = type;
	property.Address = address;
	property.MissingRule = missingRule;
	
	Properties.Add(property);
}

bool GCSVReader::ParseHeader(StringView& csv, Array<int32>& outPropertyIndices, uint32 skipRows) const
{
	ScopeBenchmark _f("GCSV::ParseHeader"_sv);
	
	LastError = StringView();
	
	int32 firstLine = csv.LeftFind('\n');
	if (firstLine == -1)
	{
		LastError = "No newline found in CSV header"_sv;
		return false;
	}
	
	StringView header = csv.Substring(0, firstLine);
	csv = csv.ChopLeft(firstLine + 1);
	
	Array<StringView> headers = header.Split(',');
	Set<ColumnName> foundHeaders;
	
	for (uint32 i=0; i<headers.Num(); ++i)
	{
		StringView h = headers[i];
		h.Trim();
		if (h.Size() == 0)
		{
			LastError = String::Format("The '{}'th header is empty"_sv, i);
			return false;
		}
		
		ColumnName column;
		column.Name = h;
		if (foundHeaders.Contains(column))
		{
			LastError = String::Format("Duplicate column '{}'"_sv, h);
			return false;
		}
		foundHeaders.Add(column);
		
		const uint32* propertyIndex = ColumnNameToPropertyIndex.Find(column);
		if (propertyIndex == nullptr)
		{
			outPropertyIndices.Add(-1);
		}
		else
		{
			outPropertyIndices.Add((int32)*propertyIndex);
		}
	}
	
	// We may have properties that are required but not in the CSV
	for (uint32 i=0; i<Properties.Num(); ++i)
	{
		if (Properties[i].MissingRule == EPropertyMissingRules::ClassDefault)
			continue;
		
		if (!outPropertyIndices.Contains(i))
		{
			LastError = String::Format("Missing column '{}'"_sv, Properties[i].Name);
			return false;
		}
	}
	
	while (skipRows > 0)
	{
		--skipRows;
		
		int32 rowEnd = csv.LeftFind('\n');
		if (rowEnd == -1)
		{
			return false;
		}
		csv = csv.ChopLeft(rowEnd + 1);
	}
	
	return true;
}

bool GCSVReader::ParseRow(StringView& csv, uint32 rowCount, void* data,
	const void* classDefault, const Array<int32>& propertyIndices, bool& bError) const
{
	int32 rowEnd = csv.LeftFind('\n');
	int32 lineEnd = rowEnd + 1;
	if (rowEnd == -1)
	{
		rowEnd = csv.Size();
		lineEnd = rowEnd;
	}
	
	if (rowEnd <= 0)
	{
		// End of content
		return false;
	}
	
	StringView row = csv.Substring(0, rowEnd);
	csv = csv.ChopLeft(lineEnd);
	
	uint32 currentColumn = 0;
	
	while (currentColumn < propertyIndices.Num())
	{
		StringView cell = StringView();
		
		while (StringUtil::IsWhitespace(row.FirstChar()))
		{
			row = row.ChopLeft(1);
		}
		
		const uint8 firstChar = row.FirstChar();
		bool bTrimCell = true;
		
		if (firstChar == '"')
		{
			// Parsing a quote
			bTrimCell = false;
			row = row.ChopLeft(1);
			uint32 length = 0;
			
			while (true)
			{
				int32 quote = row.LeftFind('"', length);
				if (quote == -1)
				{
					bError = true;
					LastError = String::Format("No closing quote found in row '{}'"_sv, row);
					return false;
				}
				
				if (quote > 0 && row.At(quote - 1) == '\\')
				{
					length = quote + 1;
					continue;
				}
				
				cell = row.Substring(0, quote);
				row = row.ChopLeft(quote + 1);
				
				while (StringUtil::IsWhitespace(row.FirstChar()))
				{
					row = row.ChopLeft(1);
				}
				if (row.FirstChar() == ',')
				{
					row = row.ChopLeft(1);
				}
				else if (row.FirstChar() == '\0')
				{
					row = StringView();
				}

				break;
			}
		}
		else if (firstChar == ',')
		{
			// Empty cell
			bTrimCell = false;
			row = row.ChopLeft(1);
		}
		else if (firstChar == '\0')
		{
			// End of row
			bTrimCell = false;
			row = StringView();
		}
		else
		{
			// Parsing a regular cell
			int32 comma = row.LeftFind(',');
			if (comma == -1)
			{
				cell = row;
				row = StringView();
			}
			else
			{
				cell = row.Substring(0, comma);
				row = row.ChopLeft(comma + 1);
			}
		}
		
		if (bTrimCell)
		{
			cell.Trim();
		}
		
		int32 propertyIndex = propertyIndices[currentColumn];
		if (propertyIndex != -1)
		{
			const Property& property = Properties[propertyIndex];
			
			bool bRequired = property.MissingRule == EPropertyMissingRules::Error;
			if (property.MissingRule == EPropertyMissingRules::CopyPrevious && rowCount == 0)
			{
				bRequired = true;
			}
			
			union {
				bool b;
				int32 i;
				uint32 u;
				f32 f;
				StringView s;
			} value = {};
			
			uint8 propertyBytes = 0;
			bool bParsed = true;
			
			if (property.Type == EPropertyType::Bool)
			{
				if (cell == "1"_sv ||
					StringUtil::IsEqualCaseInsensitive(cell, "true"_sv) ||
					StringUtil::IsEqualCaseInsensitive(cell, "yes"_sv) ||
					StringUtil::IsEqualCaseInsensitive(cell, "y"_sv) ||
					StringUtil::IsEqualCaseInsensitive(cell, "on"_sv) ||
					StringUtil::IsEqualCaseInsensitive(cell, "enabled"_sv))
				{
					value.b = true;
				}
				else if (cell == "0"_sv ||
					StringUtil::IsEqualCaseInsensitive(cell, "false"_sv) ||
					StringUtil::IsEqualCaseInsensitive(cell, "no"_sv) ||
					StringUtil::IsEqualCaseInsensitive(cell, "n"_sv) ||
					StringUtil::IsEqualCaseInsensitive(cell, "off"_sv) ||
					StringUtil::IsEqualCaseInsensitive(cell, "disabled"_sv))
				{
					value.b = false;
				}
				else
				{
					bParsed = false;
					if (bRequired)
					{
						bError = true;
						LastError = String::Format("Cannot parse '{}' as bool"_sv, cell);
						return false;
					}
				}
			}
			else if (property.Type == EPropertyType::Int32 ||
					property.Type == EPropertyType::UInt32)
			{
				int64 result = 0;
				if (!cell.ParseAsInt(result, 10))
				{
					bParsed = false;
					if (bRequired)
					{
						bError = true;
						LastError = String::Format("Cannot parse '{}' as int"_sv, cell);
						return false;
					}
				}
				else
				{
					int64 min = int64(property.Type == EPropertyType::Int32 ? (Traits::Limits<int32>::Min) : 0);
					int64 max = int64(property.Type == EPropertyType::Int32 ? (Traits::Limits<int32>::Max) : (Traits::Limits<uint32>::Max));
					
					if (result < min || result > max)
					{
						bError = true;
						LastError = String::Format("Value '{}' out of range for int"_sv, result);
						return false;
					}
					
					if (property.Type == EPropertyType::Int32)
					{
						value.i = (int32)result;
					}
					else
					{
						value.u = (uint32)result;
					}
				}
				propertyBytes = 4;
			}
			else if (property.Type == EPropertyType::Float)
			{
				// TODO (HvdK)
				bError = true;
				LastError = "Float parsing not yet supported"_sv;
				return false;
			}
			else if (property.Type == EPropertyType::StringView)
			{
				if (cell.Size() == 0)
				{
					bParsed = false;
					if (bRequired)
					{
						bError = true;
						LastError = "Empty string not allowed"_sv;
						return false;
					}
				}
				else
				{
					value.s = cell;
				}
				propertyBytes = sizeof(StringView);
			}
			else
			{
				bError = true;
				CHECK(false && "Unknown property type");
				LastError = "Unknown property type"_sv;
				return false;
			}
			
			if (bParsed)
			{
				Memory::Copy(&value, reinterpret_cast<uint8*>(data) + (uint64)property.Address, propertyBytes);
			}
			else if (property.MissingRule == EPropertyMissingRules::ClassDefault)
			{
				Memory::Copy(reinterpret_cast<const uint8*>(classDefault)+ (uint64)property.Address, reinterpret_cast<uint8*>(data) + (uint64)property.Address, propertyBytes);
			}
			// If missing rule is CopyPrevious, we don't need to do anything
		}
		
		currentColumn += 1;
	}
	
	if (row.Trimmed().Size() > 0)
	{
		bError = true;
		LastError = String::Format("Unexpected data in row: '{}'"_sv, row);
		return false;
	}
	
	return true;
}

StringView GCSVReader::GetError()
{
	return LastError.AsView();
}

void GCSVReader::ColumnName::Hash(Hasher& hasher) const
{
	hasher.Add(Name.Size());
	const char8* ptr = Name.Data();
	const uint32 size = Name.Size();
	for (uint32 i=0; i<size; ++i)
	{
		hasher.Add(ptr[i]);
	}
}

bool GCSVReader::ColumnName::operator==(const ColumnName& other) const
{
	return Name == other.Name;
}