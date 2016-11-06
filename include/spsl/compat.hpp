/**
 * @file	Special Purpose Strings Library: compat.hpp
 * @author  Daniel Evers
 * @brief   Compatibility stuff
 * @license MIT
 */

#ifndef SPSL_COMPAT_HPP_
#define SPSL_COMPAT_HPP_

#include <system_error>

#define SPSL_HAS_NOEXCEPT
#define SPSL_HAS_DEFAULT_MOVE
#define SPSL_HAS_CONSTEXPR_ARRAY

#ifdef _MSC_VER // Windows + Visual Studio

// Visual Studio has no ssize_t ...
#include <crtdefs.h>
typedef intptr_t ssize_t;

// noexcept and consexpr are supported since Visual Studio 2015
#if _MSC_VER < 1900
#define noexcept
#define constexpr const
#undef SPSL_HAS_NOEXCEPT
#undef SPSL_HAS_DEFAULT_MOVE
#endif

// can't use constexpr values in array definitions...
#undef SPSL_HAS_CONSTEXPR_ARRAY

// the min/max macros from Windows break a lot of STL stuff...
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>

namespace spsl
{
namespace os
{
/// protect an area of memory from being swapped
/// (TODO: not perfect - see https://blogs.msdn.microsoft.com/oldnewthing/20071106-00/?p=24573)
inline void lockMemory(void* addr, size_t len)
{
    if (VirtualLock(addr, len) == 0)
        throw std::system_error(GetLastError(), std::system_category());
}
/// remove the protection again
inline void unlockMemory(void* addr, size_t len)
{
    // FIXME: Windows doesn't have a lock count and might unlock too early if we release another
    // chunk of memory!
    // -> need to manage pages!
    if (VirtualUnlock(addr, len) == 0 &&
            GetLastError() != ERROR_NOT_LOCKED)
        throw std::system_error(GetLastError(), std::system_category());
}
}
}

#else // Linux

#include <sys/mman.h>
#include <cerrno>

namespace spsl
{
namespace os
{
/// protect an area of memory from being swapped
inline void lockMemory(void* addr, size_t len)
{
    // TODO: only works for page-aligned memory blocks - we need to allocate full pages...
#if 0
    // since Linux 3.4
#ifdef MADV_DONTDUMP
    // exclude from core dumps
    if (0 != madvise(addr, len, MADV_DONTDUMP))
        throw std::system_error(errno, std::generic_category());
#endif
#endif
    // lock into RAM - may not be swapped
    if (0 != mlock(addr, len))
        throw std::system_error(errno, std::generic_category());
}
/// remove the protection again
inline void unlockMemory(void* addr, size_t len)
{
    // TODO: only works for page-aligned memory blocks
#if 0
    // since Linux 3.4
#ifdef MADV_DODUMP
    // include in core dumps again
    if (0 != madvise(addr, len, MADV_DODUMP))
        throw std::system_error(errno, std::generic_category());
#endif
#endif
    // unlock - may be swapped again
    if (0 != munlock(addr, len))
        throw std::system_error(errno, std::generic_category());
}
}
}

#endif

// define a secure memset function
#endif /* SPSL_COMPAT_HPP_ */
