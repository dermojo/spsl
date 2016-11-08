# SPSL - Special Purpose Strings Library

[![Build Status](https://travis-ci.org/dermojo/spsl.svg?branch=master)](https://travis-ci.org/dermojo/spsl)

SPSL is a header-only library for "special" string implementations. It contains the following
string class templates:
* `ArrayString`/`ArrayStringW`: Purely stack based string, similar to "plain" C arrays.
* `PasswordString`/`PasswordStringW`: String variant for sensitive data. All memory used by
  this string implementation is locked in RAM (i.e. it may not be swapped to disk) and is
  wiped before it's returned to the OS.

If you're interested in the motivation behind these templates, check out the
[Motivation](docs/Motivation.md) document.
If you're interested in the nitty gritty details of how this library works, head over to
[Implementation](docs/Implementation.md).


## Getting started

SPSL is a header-only library, so you don't need to compile anything. Just download it, include
it in your project and you're good to go:

    #include <spsl.hpp>
    spsl::ArrayString<64> a1("I'm a stack-based string and can store 64 characters + NUL");
    spsl::ArrayStringW<32> a2(L"Wide characters are ok, too.");
    spsl::PasswordString pw1("I'm a secret!");
    spsl::PasswordStringW pw2(L"Dream on...");

However, there are a lot of unit tests in this library. If you want to run them (to check
compatibility with your platform or if you played with some library code), the next sections
are for you.

### Dependencies

The SPSL itself only relies on the STL and has no dependencies. To compile the tests, you'll need:
* [CMake](https://cmake.org/) 3.2 or higher
* [GoogleTest](https://github.com/google/googletest) 1.8

Supported compilers (we'll, the ones I've tested) include GCC 5.4 and clang 3.8 on Linux, 
Microsoft Visual Studio 2013 (Update 5) and Microsoft Visual Studio 2015.


## Running the tests

SPSL comes with a lot of unit tests. They are compiled into the `testlib` executable, which can
be run standalone or from cmake/ctest. 

To build on Linux, simply run CMake and execute the tests:
    mkdir build
    cd build
    cmake ..
    cmake runtest    # or run ./testlib
(Note: "runtest" is a custom build target. You can use the default `cmake test`, but I prefer
more verbose output :-) .)

For Windows, there is a helper batch script (`run_cmake.bat`) to run cmake with the appropriate
options. You probably need to edit it to change the paths for GoogleTest. Then open a command prompt
and execute (note: cmake needs to be in your PATH):
    md build_msvc
    cd build_msvc
    ..\cmake.bat
Now there should be a "spsl.sln" solution file that you can use to compile the tests with
Visual Studio. When done, execute `RelWithDebInfo\testlib.exe` to run them.


## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

tl;dr: Feel free to use and modify the code or to include it in your commercial application.


## Acknowledgements

This library uses the MurmurHash3 hash functions, copied thankfully from
[https://github.com/aappleby/smhasher](https://github.com/aappleby/smhasher). Thanks a lot! 
