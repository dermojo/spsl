#
# Special Purpose Strings Library
#
# Requires CMake 3.2 or higher
cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
project(spsl VERSION 2.0 LANGUAGES CXX)

# Release version with debug info: Compiles with optimizations, and if a unit test crashes,
# there may be hope for analysis...
#set(CMAKE_BUILD_TYPE RelWithDebInfo)

# Setup testing
enable_testing()

#
# Build targets
#

# Every library has unit tests, of course
add_executable(testlib
    test/test_storagearray.cpp
    test/test_storagepassword.cpp
    test/test_stringbase.cpp
    test/test_stringbase_erase.cpp
    test/test_stringbase_find.cpp
    test/test_stringbase_replace.cpp
    test/test_stringcore.cpp
    test/test_stringcore_access.cpp
    test/test_stringcore_assign.cpp
    test/test_stringcore_construct.cpp
    test/test_pagealloc.cpp
    test/test_main.cpp
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
target_include_directories(testlib SYSTEM PUBLIC extern)
target_include_directories(example PUBLIC include)

set_property(TARGET testlib PROPERTY CXX_STANDARD 17)
set_property(TARGET testlib PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET example PROPERTY CXX_STANDARD 17)
set_property(TARGET example PROPERTY CXX_STANDARD_REQUIRED ON)

# We want a lot of warnings!
if(MSVC)
    target_compile_options(testlib PUBLIC /W4 /WX)
    # Prevent deprecation errors for std::tr1 in googletest
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /D_SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pipe")
    target_compile_options(testlib PUBLIC -Wall -Werror -Wextra -pedantic
                                          -Wunused -Wredundant-decls -Wunreachable-code
                                          -Wold-style-cast -Wshadow
                                          -Wconversion -Wsign-conversion -Wno-conversion-null
                                          -Wcast-align)
    # These are enabled on Clang and don't help...
    if("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
        target_compile_options(testlib PUBLIC -Wno-missing-braces
                                              -Wno-gnu-zero-variadic-macro-arguments)
    endif()
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

option(ENABLE_COVERAGE "Enable code coverage flags" OFF)

if(ENABLE_COVERAGE)
    set(CMAKE_CXX_FLAGS    "${CMAKE_CXX_FLAGS}    -fprofile-arcs -ftest-coverage")
    set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -fprofile-arcs -ftest-coverage")
endif()


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
    set(CLANG_FORMAT clang-format-6.0)
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
    set(CLANG_TIDY clang-tidy-6.0)
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
