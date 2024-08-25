cmake_minimum_required(VERSION 3.10)
project(HK)

add_compile_definitions($<$<CONFIG:Debug>:_DEBUG_>)
add_compile_definitions($<$<CONFIG:Release>:_RELEASE_>)
add_compile_definitions(_CRT_SECURE_NO_WARNINGS)

# Append to compile flags
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /P /E")

add_subdirectory(HK)
add_subdirectory(Test)

# target_compile_options(HK PRIVATE /P)
# target_compile_options(Compiler PRIVATE /P)

# Add an additional parameter to windows builds

set_target_properties(Test PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")


## CMakeList.txt : CMake project for GBL, include source and define
## project specific logic here.
##
#cmake_minimum_required (VERSION 3.8)
#
## Enable Hot Reload for MSVC compilers if supported.
#if (POLICY CMP0141)
#  cmake_policy(SET CMP0141 NEW)
#  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
#endif()
#
#cmake_minimum_required(VERSION 3.13)
#
#project (GBL)
#
#add_subdirectory(Compiler)
#add_subdirectory(HK)
#add_subdirectory(Test)
#
## Add the static library HK
#
#
## Add source to this project's executable.
#add_executable (GBL_Compiler "Compiler/main.cpp")
#add_executable (GBL_Test "Test/main.cpp" )
#
#if (CMAKE_VERSION VERSION_GREATER 3.12)
#  set_property(TARGET GBL_Compiler PROPERTY CXX_STANDARD 20)
#  set_property(TARGET GBL_Test PROPERTY CXX_STANDARD 20)
#endif()
#
## TODO: Add tests and install targets if needed.
#