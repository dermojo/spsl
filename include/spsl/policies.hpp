/**
 * @file    Special Purpose Strings Library: polices.hpp
 * @author  Daniel Evers
 * @brief   Policy classes
 * @license MIT
 */

#ifndef SPSL_POLICIES_HPP_
#define SPSL_POLICIES_HPP_

#include <algorithm>
#include <stdexcept>

namespace spsl
{
namespace policy
{
/**
 * Policies in this namespace deal with possible buffer overflows. They are called by
 * @c StorageArray to check for and possibly handle overflows.
 *
 * All classes must implement the following (static) template functions:
 *   (1) void checkReserve<size_type>(size_type cap, size_type max)
 *   (2) size_type checkAssign<char_type, size_type>(const char_type* s, size_type n, size_type max)
 *   (3) size_type checkAssign<char_type, size_type>(size_type n, char_type ch, size_type max)
 *   (4) size_type checkAppend<char_type, size_type>(const char_type* s, size_type n,
 *                                                   size_type size, size_type max)
 *   (5) size_type checkAppend<char_type, size_type>(size_type n, char_type ch,
 *                                                   size_type size, size_type max)
 *
 * Function (1) is called from within reserve() with the requested capacity and the maximum size.
 * Since reserve() itself is a no-op, this function returns nothing.
 *
 * Functions (2) and (3) are called from within assign() and similar methods with the string
 * or the characters to assign as well as the maximum capacity. They have to return a "proper"
 * number of characters to assign or must not return at all.
 *
 * Functions (4) and (5) are called from within append() and similar methods with the string
 * or the characters to append as well as the current size and the maximum capacity. They have to
 * return a "proper" number of characters to append or must not return at all.
 */
namespace overflow
{

/**
 * Policy class that truncates strings to fit them into a buffer. All functions are constexpr
 * and noexcept.
 */
struct Truncate
{
    template <typename size_type>
    static constexpr bool checkReserve(size_type, size_type) noexcept
    {
        // this is a hack: constexpr functions using return type 'void' are possible since C++14
        return true;
    }
    template <typename char_type, typename size_type>
    static constexpr size_type checkAssign(const char_type*, size_type n, size_type max) noexcept
    {
        return std::min(n, max);
    }
    template <typename char_type, typename size_type>
    static constexpr size_type checkAssign(size_type n, char_type, size_type max) noexcept
    {
        return std::min(n, max);
    }
    template <typename char_type, typename size_type>
    static constexpr size_type checkAppend(const char_type*, size_type n, size_type size,
                                           size_type max) noexcept
    {
        return std::min(n, max - size);
    }
    template <typename char_type, typename size_type>
    static constexpr size_type checkAppend(size_type n, char_type, size_type size,
                                           size_type max) noexcept
    {
        return std::min(n, max - size);
    }
};

/// Policy class that throws an exception if a string is too long.
struct Throw
{
    template <typename size_type>
    static void checkReserve(size_type cap, size_type max)
    {
        if (cap > max)
            throw std::length_error("requested capacity exceeds maximum");
    };
    template <typename char_type, typename size_type>
    static size_type checkAssign(const char_type*, size_type n, size_type max)
    {
        if (n > max)
            throw std::length_error("string length exceeds maximum capacity");
        return n;
    }
    template <typename char_type, typename size_type>
    static size_type checkAssign(size_type n, char_type, size_type max)
    {
        if (n > max)
            throw std::length_error("string length exceeds maximum capacity");
        return n;
    }
    template <typename char_type, typename size_type>
    static size_type checkAppend(const char_type*, size_type n, size_type size, size_type max)
    {
        if (size + n > max)
            throw std::length_error("string length exceeds maximum capacity");
        return n;
    }
    template <typename char_type, typename size_type>
    static size_type checkAppend(size_type n, char_type, size_type size, size_type max)
    {
        if (size + n > max)
            throw std::length_error("string length exceeds maximum capacity");
        return n;
    }
};
}
}
}


#endif /* SPSL_POLICIES_HPP_ */
