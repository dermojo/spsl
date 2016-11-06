#
# Special Purpose Strings Library
#
# At LEAST 3.2 but newer is better
cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
project(spsl VERSION 0.1 LANGUAGES CXX)

# Release version with debug info: Compiles with optimizations, and if a unit test crashes,
# there may be hope for analysis...
set(CMAKE_BUILD_TYPE RelWithDebInfo)

# Setup testing using GTEST
enable_testing()
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIR})

#
# Build targets
#

# Every library has unit tests, of course
add_executable(testlib
    test/test_storagearray.cpp
    test/test_storagepassword.cpp
    test/test_stringcore.cpp
    test/test_stringbase.cpp
    test/test_traits.cpp
    )
add_executable(example
    test/example.cpp
    )

add_test(testlib testlib)

# the default for ctest is very short... also the dependency to re-build testlib is missing
add_custom_target(runtest COMMAND ./testlib)
add_dependencies(runtest testlib)

#
# Compiler and linker options
#

# Define headers for this library. PUBLIC headers are used for
# compiling the library, and will be added to consumers' build
# paths.
target_include_directories(testlib PUBLIC include)
target_include_directories(example PUBLIC include)

# Link test executable against gtest & gtest_main
target_link_libraries(testlib ${GTEST_LIBRARY} ${GTEST_MAIN_LIBRARY})

set_property(TARGET testlib PROPERTY CXX_STANDARD 11)
set_property(TARGET testlib PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET example PROPERTY CXX_STANDARD 11)
set_property(TARGET example PROPERTY CXX_STANDARD_REQUIRED ON)

# uncomment to use Clang instead of GCC
#set(CMAKE_CXX_COMPILER "/usr/bin/clang++")

# We want a lot of warnings!
string(TOLOWER "${CMAKE_BUILD_TOOL}" MY_BUILDTOOL)
if(MY_BUILDTOOL MATCHES "(msdev|devenv|nmake|msbuild)")
    target_compile_options(testlib PUBLIC /W4 /WX)
else()
    target_compile_options(testlib PUBLIC -Wall -Weffc++ -Werror)
    # These are enabled on Clang and don't help...
    target_compile_options(testlib PUBLIC -Wno-missing-braces)
    target_link_libraries(testlib pthread)
endif()

# use this to enable C++17 tests (esp. string_view) with GCC/Clang
# (but comment the CXX_STANDARD* settings above)
#target_compile_options(testlib PUBLIC -std=c++1z)
