cmake_minimum_required(VERSION 3.10.0)

project(HK VERSION 0.1.0)

add_library(HK
	Delegate.cpp
	Random.cpp
	Allocators/Memory.cpp
	Allocators/StringPool.cpp
	Bench/Bench.cpp
	Common/CompilerMacros.cpp
	Common/Math.cpp
	Common/StringUtil.cpp
	Common/Types.cpp
	Common/Print.cpp
	Containers/View/ArrayView.cpp
	Containers/View/StringView.cpp
	Containers/Array.cpp
	Containers/BitArray.cpp
	File/CSV.cpp
	File/File.cpp
	File/File_Linux.cpp
	File/File_Windows.cpp
	File/FilePath.cpp
	File/FilePath_Linux.cpp
	File/FilePath_Windows.cpp
	Strings/String.cpp
	Util/DualType.cpp
	Util/Platform_Linux.cpp
	Util/Platform_Windows.cpp
	Util/Hasher.cpp
	Util/Out.cpp)

target_include_directories(HK PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})