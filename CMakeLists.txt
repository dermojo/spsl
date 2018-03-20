#
# Special Purpose Strings Library
#
# Requires CMake 3.2 or higher (use PPA for Travis CI)
cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
project(spsl VERSION 1.3 LANGUAGES CXX)

# Release version with debug info: Compiles with optimizations, and if a unit test crashes,
# there may be hope for analysis...
#set(CMAKE_BUILD_TYPE RelWithDebInfo)

# Setup testing using GTEST
enable_testing()
find_package(GTest REQUIRED)
include_directories(SYSTEM ${GTEST_INCLUDE_DIR})

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
    test/test_pagealloc.cpp
    )
add_executable(example
    test/example.cpp
    )

add_test(testlib testlib)

# the default for ctest is very short... also the dependency to re-build testlib is missing
add_custom_target(runtest COMMAND ./testlib${CMAKE_EXECUTABLE_SUFFIX})
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
    # Prevent deprecation errors for std::tr1 in googletest
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /D_SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING")
else()
    target_compile_options(testlib PUBLIC -Wall -Werror -Wextra -pedantic
                                          -Wunused -Wredundant-decls -Wunreachable-code
                                          -Wold-style-cast -Wshadow
                                          -Wconversion -Wsign-conversion -Wno-conversion-null
                                          -Wcast-align)
    # These are enabled on Clang and don't help...
    target_compile_options(testlib PUBLIC -Wno-missing-braces)
    target_link_libraries(testlib pthread)
endif()

option(ENABLE_ASAN "Enable address sanitizer instrumentation" OFF)

if(ENABLE_ASAN)
    set(CMAKE_CXX_FLAGS    "${CMAKE_CXX_FLAGS}    -fsanitize=address")
    set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -fsanitize=address")
endif()

option(ENABLE_UBSAN "Enable UB sanitizer instrumentation" OFF)

if(ENABLE_UBSAN)
    set(CMAKE_CXX_FLAGS    "${CMAKE_CXX_FLAGS}    -fsanitize=undefined")
    set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -fsanitize=undefined")
endif()

# use this to enable C++17 tests (esp. string_view) with GCC/Clang
# (but comment the CXX_STANDARD* settings above)
#target_compile_options(testlib PUBLIC -std=c++1z)

# run clang-format on all files
list(APPEND CLANG_FORMAT_ARGS
    -style=file
    -i
    ${CMAKE_SOURCE_DIR}/include/*.hpp
    ${CMAKE_SOURCE_DIR}/include/spsl/*.hpp
    ${CMAKE_SOURCE_DIR}/test/*.hpp
    ${CMAKE_SOURCE_DIR}/test/*.cpp
)

if(NOT DEFINED CLANG_FORMAT)
    set(CLANG_FORMAT clang-format-4.0)
endif()
add_custom_target(
    format
    COMMAND ${CLANG_FORMAT} ${CLANG_FORMAT_ARGS}
    COMMENT "running clang-format"
)

# clang tidy support (using version 4.0 atm)
# (since this is a header-only library, it's sufficient to check a single source that includes
#  everything)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
if(NOT DEFINED CLANG_TIDY)
    set(CLANG_TIDY clang-tidy-4.0)
endif()
list(APPEND CLANG_TIDY_ARGS
    -p .
    ${CMAKE_SOURCE_DIR}/test/example.cpp
)

add_custom_target(
    tidy
    COMMAND ${CLANG_TIDY} ${CLANG_TIDY_ARGS}
    COMMENT "running clang tidy"
)
