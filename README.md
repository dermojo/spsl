# SPSL - Special Purpose Strings Library

[![Travis-CI status](https://travis-ci.org/dermojo/spsl.svg?branch=master)](https://travis-ci.org/dermojo/spsl)
[![AppVeyor status](https://ci.appveyor.com/api/projects/status/github/dermojo/spsl?branch=master&svg=true)](https://ci.appveyor.com/project/dermojo/spsl)
[![Coverity Scan Build Status](https://scan.coverity.com/projects/11354/badge.svg)](https://scan.coverity.com/projects/dermojo-spsl)
[![codecov](https://codecov.io/gh/dermojo/spsl/branch/master/graph/badge.svg)](https://codecov.io/gh/dermojo/spsl)

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

```c++
#include <spsl.hpp>
spsl::ArrayString<64> a1("I'm a stack-based string and can store 64 characters + NUL");
spsl::ArrayStringW<32> a2(L"Wide characters are ok, too.");
spsl::PasswordString pw1("I'm a secret!");
spsl::PasswordStringW pw2(L"Dream on...");
```

However, there are a lot of unit tests in this library. If you want to run them (to check
compatibility with your platform or if you played with some library code), the next sections
are for you.

### Dependencies

The SPSL itself only relies on the STL and has no dependencies. To compile the tests, you'll need:
* [CMake](https://cmake.org/) 3.2 or higher
* [GoogleTest](https://github.com/google/googletest) (as [git submodule](https://git-scm.com/docs/git-submodule))

Supported compilers (we'll, the ones I've tested) include GCC 4.9+ and clang 3.8+
on Linux and Microsoft Visual Studio 2015.

Make sure to clone this repository with:
```bash
git clone --recurse-submodules https://github.com/dermojo/spsl.git
```


## Running the tests

SPSL comes with a lot of unit tests. They are compiled into the `testlib` executable, which can
be run standalone or from cmake/ctest.

To build and run all tests, run the usual CMake commands:

```bash
mkdir build
cd build
cmake ..
cmake --build .
ctest -VV
```

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

tl;dr: Feel free to use and modify the code or to include it in your commercial application.


## Acknowledgments

This library uses the MurmurHash3 hash functions, copied thankfully from
[https://github.com/aappleby/smhasher](https://github.com/aappleby/smhasher). Thanks a lot!
