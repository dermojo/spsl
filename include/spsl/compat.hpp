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
#define SPSL_HAS_CONSTEXPR_ARRAY

#ifdef _WIN32   // Windows
#ifdef _MSC_VER // Visual Studio

// Visual Studio has no ssize_t ...
#include <crtdefs.h>
typedef intptr_t ssize_t;

// noexcept and constexpr are supported since Visual Studio 2015
#if _MSC_VER < 1900
#define noexcept
#define constexpr const
#undef SPSL_HAS_NOEXCEPT
#endif

// can't use constexpr values in array definitions...
#undef SPSL_HAS_CONSTEXPR_ARRAY

#endif // _MSC_VER

// the min/max macros from Windows break a lot of STL stuff...
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>

namespace spsl
{
namespace os
{

/// @return the system's page size
inline std::size_t getPageSize()
{
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    if (si.dwPageSize == 0)
        throw std::runtime_error("unknown page size");
    return static_cast<std::size_t>(si.dwPageSize);
}

/**
 * Allocates memory that is aligned to the given page size.
 * @param[in] pageSize      the page size
 * @param[in] n             the required size
 * @return memory address or nullptr
 */
inline void* allocatePageAligned(std::size_t pageSize, std::size_t n)
{
    (void)pageSize;
    return VirtualAlloc(NULL, n, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
}

/**
 * Release page-aligned memory.
 * @param[in] addr      the memory address
 */
inline void deallocatePageAligned(void* addr)
{
    VirtualFree(addr, 0U, MEM_RELEASE);
}

/**
 * Exclude an area of memory from core dumps.
 * * This function is a dummy *
 */
inline void disableDump(void* addr, std::size_t len, std::error_code* ec = nullptr)
{
    (void)addr;
    (void)len;
    (void)ec;
}

/**
 * Include an area of memory in core dumps again.
 * * This function is a dummy *
 */
inline void enableDump(void* addr, std::size_t len, std::error_code* ec = nullptr)
{
    (void)addr;
    (void)len;
    (void)ec;
}


/**
 * Protect an area of memory from being swapped by "locking" it into RAM.
 * @param[in] addr      the address of the memory area
 * @param[in] len       the length of the area
 * @param[out] ec       optional error code to be used instead of throwing
 * @throws std::system_error on error and if ec == nullptr
 *
 * Note: not perfect - see https://blogs.msdn.microsoft.com/oldnewthing/20071106-00/?p=24573
 * The memory area should be a page (or a multiple thereof) to avoid problems.
 */
inline void lockMemory(void* addr, std::size_t len, std::error_code* ec = nullptr)
{
    if (VirtualLock(addr, len) == 0)
    {
        std::error_code err(static_cast<int>(GetLastError()), std::system_category());
        if (ec)
            *ec = err;
        else
            throw std::system_error(err);
    }
}
/// remove the protection again
inline void unlockMemory(void* addr, std::size_t len, std::error_code* ec = nullptr)
{
    if (VirtualUnlock(addr, len) == 0)
    {
        std::error_code err(static_cast<int>(GetLastError()), std::system_category());
        if (ec)
            *ec = err;
        else
            throw std::system_error(err);
    }
}
}
}

#else // Linux

#include <cerrno>
#include <sys/mman.h>
#include <unistd.h>

namespace spsl
{
namespace os
{
/// @return the system's page size
inline std::size_t getPageSize()
{
    long page_size = sysconf(_SC_PAGESIZE);
    if (page_size <= 0)
        throw std::runtime_error("unknown page size");
    return static_cast<std::size_t>(page_size);
}

/**
 * Allocates memory that is aligned to the given page size.
 * @param[in] pageSize      the page size
 * @param[in] n             the required size
 * @return memory address or nullptr
 */
inline void* allocatePageAligned(std::size_t pageSize, std::size_t n)
{
    void* addr;
    if (posix_memalign(&addr, pageSize, n) != 0)
        addr = nullptr;
    return addr;
}

/**
 * Release page-aligned memory.
 * @param[in] addr      the memory address
 */
inline void deallocatePageAligned(void* addr)
{
    free(addr);
}

/**
 * Exclude an area of memory from core dumps.
 * @param[in] addr      the address of the memory area
 * @param[in] len       the length of the area
 * @param[out] ec       optional error code to be used instead of throwing
 * @throws std::system_error on error and if ec == nullptr
 *
 * The memory area should be a page (or a multiple thereof) to avoid problems.
 */
inline void disableDump(void* addr, std::size_t len, std::error_code* ec = nullptr)
{
// since Linux 3.4
#ifdef MADV_DONTDUMP
    // exclude from core dumps
    if (0 != madvise(addr, len, MADV_DONTDUMP))
    {
        std::error_code err(errno, std::generic_category());
        if (ec)
            *ec = err;
        else
            throw std::system_error(err);
    }
#else
    (void)addr;
    (void)len;
    (void)ec;
#endif
}

/**
 * Include an area of memory in core dumps again.
 * @param[in] addr      the address of the memory area
 * @param[in] len       the length of the area
 * @param[out] ec       optional error code to be used instead of throwing
 * @throws std::system_error on error and if ec == nullptr
 *
 * The memory area should be a page (or a multiple thereof) to avoid problems.
 */
inline void enableDump(void* addr, std::size_t len, std::error_code* ec = nullptr)
{
// since Linux 3.4
#ifdef MADV_DODUMP
    // include in core dumps again
    if (0 != madvise(addr, len, MADV_DODUMP))
    {
        std::error_code err(errno, std::generic_category());
        if (ec)
            *ec = err;
        else
            throw std::system_error(err);
    }
#else
    (void)addr;
    (void)len;
    (void)ec;
#endif
}

/**
 * Protect an area of memory from being swapped by "locking" it into RAM.
 * @param[in] addr      the address of the memory area
 * @param[in] len       the length of the area
 * @param[out] ec       optional error code to be used instead of throwing
 * @throws std::system_error on error and if ec == nullptr
 *
 * The memory area should be a page (or a multiple thereof) to avoid problems.
 */
inline void lockMemory(void* addr, std::size_t len, std::error_code* ec = nullptr)
{
    // lock into RAM - may not be swapped
    // --> https://www.ibm.com/developerworks/library/s-data.html?n-s-311
    if (0 != mlock(addr, len))
    {
        std::error_code err(errno, std::generic_category());
        if (ec)
            *ec = err;
        else
            throw std::system_error(err);
    }
}

/// remove the protection again
inline void unlockMemory(void* addr, std::size_t len, std::error_code* ec = nullptr)
{
    // unlock - may be swapped again
    if (0 != munlock(addr, len))
    {
        std::error_code err(errno, std::generic_category());
        if (ec)
            *ec = err;
        else
            throw std::system_error(err);
    }
}
}
}

#endif

// define a secure memset function
#endif /* SPSL_COMPAT_HPP_ */
