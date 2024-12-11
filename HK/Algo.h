#pragma once

#include "Common/Types.h"
#include "Allocators/Memory.h"

namespace Algo
{
	template<typename T>
	void Sort(T* data, uint32 num, int32(*compare)(const typename RemoveReference<T>::Type&, const typename RemoveReference<T>::Type&))
	{
		// TODO (HvdK): Use a more efficient sorting algorithm
		for (uint32 i = 0; i < num; ++i) {
			for (uint32 j = num - 1; j > i; --j) {
				if (compare(data[j], data[i]) < 0) {
					T tmp = Move(data[i]);
					data[i] = Move(data[j]);
					data[j] = Move(tmp);
				}
			}
		}
	}
	
	template<typename T>
	void SortWithContext(T* data, uint32 num, int32(*compare)(const T&, const T&, void*), void* context)
	{
		// TODO (HvdK): Use a more efficient sorting algorithm
		for (uint32 i = 0; i < num; ++i) {
			for (uint32 j = num - 1; j > i; --j) {
				if (compare(data[j], data[i], context) < 0) {
					T tmp = Move(data[i]);
					data[i] = Move(data[j]);
					data[j] = Move(tmp);
				}
			}
		}
	}
	
	template<typename T>
	int32 Find(const T* data, uint32 num, const T& value, bool(*compare)(const T&, const T&))
	{
		for (uint32 i = 0; i < num; ++i) {
			if (compare(data[i], value)) {
				return i;
			}
		}
		return -1;
	}
};