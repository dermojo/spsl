/**
 * @file    Special Purpose Strings Library: spsl.hpp
 * @author  Daniel Evers
 * @brief   Project main header
 * @license MIT
 *
 * This header-only library provides string implementations for special purposes:
 *  - A string implementation based on a fixed-sized stack-based array.
 *  - A string implementation suitable for sensitive data - such as passwords.
 *
 * This functionality cannot be implemented using allocators, due to SSO (small/short
 * string optimizations) problems: The use of SSO makes a password implementation impossible,
 * because the allocator cannot wipe the memory stored inside the basic_string template. Also,
 * SSO limits the max_size() of the allocator (see the GCC implementation), therefore making
 * an array-based allocator impossible.
 *
 * This library therefore ships it's own string base class, that bases on one of the two
 * "storage" implementations. Note that "storage" != "allocator": The requirements on the
 * storage class superseed pure allocation/deallocation, hence we use a different term.
 */

#ifndef SPSL_SPSL_HPP_
#define SPSL_SPSL_HPP_

#include "spsl/storage_array.hpp"
#include "spsl/storage_password.hpp"
#include "spsl/stringbase.hpp"
#include "spsl/stringcore.hpp"

namespace spsl
{

/*
 * ArrayString / ArrayStringW:
 * These string implementations have a stack-only buffer and never allocate any memory from heap.
 * They behave like a "legacy" C-string array, but with a std::string-like interface.
 */

template <size_t MaxSize, typename OverflowPolicy = policy::overflow::Truncate>
using ArrayString = StringBase<StorageArray<char, MaxSize, OverflowPolicy>>;

template <size_t MaxSize, typename OverflowPolicy = policy::overflow::Truncate>
using ArrayStringW = StringBase<StorageArray<wchar_t, MaxSize, OverflowPolicy>>;

/*
 * PasswordString / PasswordString:
 * These string implementations may be used to store sensitive data, such as passwords. All
 * allocated memory is zero'd before returning it to the OS.
 * These strings use the "reduced" StringCore template because the more "advanced" string functions
 * like "erase" and "replace" are usually not required/useful for passwords and are therefore
 * not available here.
 */
using PasswordString = StringCore<StoragePassword<char>>;
using PasswordStringW = StringCore<StoragePassword<wchar_t>>;
} // namespace spsl


#endif /* SPSL_SPSL_HPP_ */
