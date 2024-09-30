# HK
A C++ standard library replacement that focuses on fast compile times by minimizing the amount of code present in all files. This means that not all checks and functionalities are presents that are expected from a standard library replacement, but the features in-place are to the point.
The HK library is the foundation for all my personal projects and is constantly being updated and improved. The library is designed to be as lightweight as possible.

My focus lies with game development and game tool development, slightly wrong behavior is hard to reproduce and debug, and the exact time of a bug is not always known. This is why I have chosen to use asserts in my code. The library is designed to crash on errors so that you can easily find the source of the problem, not hide the problem and cause more issues down the line.

This library has been tested on G++ 11.4.1 and MSVC 19.39.33519. It is compiled CMake and uses no modern C++ features. When compiling ensure you have a compile definition for \_DEBUG\_ or \_RELEASE\_ to enable or disable the right optimizations.

Please note that this library is not designed to be a drop-in replacement for the standard library, but rather a lightweight alternative. It is also incomplete and will be updated as I need more functionality.