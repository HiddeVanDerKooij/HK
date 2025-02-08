cmake_minimum_required(VERSION 3.10.0)

project(HK VERSION 0.1.0)

add_library(HK
	Delegate.cpp
	Random.cpp
	Allocators/Memory.cpp
	Allocators/Memory.h
	Allocators/Pool.h
	Allocators/StringPool.cpp
	Allocators/StringPool.h
	Bench/Bench.cpp
	Common/CompilerMacros.cpp
	Common/CompilerMacros.h
	Common/Math.cpp
	Common/StringUtil.cpp
	Common/Types.cpp
	Common/Types.h
	Common/Print.cpp
	Common/Print.h
	Containers/View/ArrayView.cpp
	Containers/View/ArrayView.h
	Containers/View/StringView.cpp
	Containers/View/StringView.h
	Containers/Array.cpp
	Containers/BitArray.cpp
	File/CSV.cpp
	File/File.cpp
	File/File_Linux.cpp
	File/File_Windows.cpp
	File/FilePath.cpp
	File/FilePath_Linux.cpp
	File/FilePath_Windows.cpp
	Strings/String.h
	Strings/String.cpp
	Util/DualType.h
	Util/DualType.cpp
	Util/ErrorOr.h
	Util/Platform_Linux.cpp
	Util/Platform_Windows.cpp
	Util/Out.h
	Util/Out.cpp)

target_include_directories(HK PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})