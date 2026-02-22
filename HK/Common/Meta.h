#pragma once

#include "Common/Types.h"

class Meta {
public:
	template<typename T>
	struct IsPointer {
		static constexpr bool Value = false;
	};
	template<typename T>
	struct IsPointer<T*> {
		static constexpr bool Value = true;
	};
	template<typename T>
	struct IsReference {
		static constexpr bool Value = false;
	};
	template<typename T>
	struct IsReference<T&> {
		static constexpr bool Value = true;
	};
	template<typename T>
	struct IsConst {
		static constexpr bool Value = false;
	};
	template<typename T>
	struct IsConst<const T> {
		static constexpr bool Value = true;
	};
	
	template<typename T1, typename T2>
	struct IsSame {
		static constexpr bool Value = false;
	};
	template<typename T>
	struct IsSame<T, T> {
		static constexpr bool Value = true;
	};
	
	// Returns the number of arguments in the list Types.
	template<typename... Types>
	static constexpr int32 GetNumTypes() {
		return sizeof...(Types);
	}
	
	// Get the max of two sizes
	template<int32 I1, int32 I2>
	struct Max {
		static constexpr int32 Value = (I1 > I2) ? I1 : I2;
	};
	
private:
	template<typename... Types>
	struct GetMaxSizeImpl;
	
	template<typename T>
	struct GetMaxSizeImpl<T> {
		static constexpr int32 Value = sizeof(T);
	};
	
	template<typename T1, typename T2>
	struct GetMaxSizeImpl<T1, T2> {
		static constexpr int32 Value = Max<sizeof(T1), sizeof(T2)>::Value;
	};
	
	template<typename T, typename... Types>
	struct GetMaxSizeImpl<T, Types...> {
		static constexpr int32 Value = Max<sizeof(T), GetMaxSizeImpl<Types...>::Value>::Value;
	};
	
	template<typename T, typename... Types>
	struct GetIndexImpl;
	
	template<typename T>
	struct GetIndexImpl<T> {
		static constexpr int32 Value = -1;
	};
	
	template<typename T, typename... Types>
	struct GetIndexImpl<T, T, Types...> {
		static constexpr int32 Value = 0;
	};
	
	template<typename T, typename U, typename... Types>
	struct GetIndexImpl<T, U, Types...> {
		static constexpr int32 Value = (GetIndexImpl<T, Types...>::Value == -1) ? -1 : GetIndexImpl<T, Types...>::Value + 1;
	};
	
public:
	
	// Get the size of largest type in the list Types.
	template<typename... Types>
	struct GetMaxSize {
		static constexpr int32 Value = GetMaxSizeImpl<Types...>::Value;
	};
	
	// Given template T and a list of types Types, this function
	// returns the index of T in Types.
	// If T is not in Types, it returns -1.
	template<typename T, typename... Types>
	static constexpr int32 GetIndex() {
		return GetIndexImpl<T, Types...>::Value;
	}
};
