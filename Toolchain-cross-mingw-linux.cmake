# the name of the target operating system
set(CMAKE_SYSTEM_NAME Windows)

# Choose an appropriate compiler prefix

set(COMPILER_PREFIX "/usr/bin/x86_64-w64-mingw32")
set(COMPILER_POSTFIX "")

# which compilers to use for C and C++
set(CMAKE_RC_COMPILER ${COMPILER_PREFIX}-windres)
set(CMAKE_C_COMPILER ${COMPILER_PREFIX}-gcc-posix)
set(CMAKE_CXX_COMPILER ${COMPILER_PREFIX}-g++-posix)

# the target environment is located here
set(CMAKE_FIND_ROOT_PATH  /usr/${COMPILER_PREFIX} ) #${USER_ROOT_PATH})

# adjust the default behavior of the FIND_XXX() commands:
# search headers and libraries in the target environment, search
# programs in the host environment
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
