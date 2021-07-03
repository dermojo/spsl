/**
 * @author  Daniel Evers
 * @brief   Common test header
 * @license MIT
 */

#ifndef TEST_TESTS_HPP_
#define TEST_TESTS_HPP_

#include <cstddef>
#include <tuple>

#include "catch.hpp"

#include "spsl.hpp"


// all string types we want to test
using StringBaseTestTypes =
  std::tuple<spsl::StringBase<spsl::StorageArray<char, 64, spsl::policy::overflow::Truncate>>,
             spsl::StringBase<spsl::StorageArray<char, 64, spsl::policy::overflow::Throw>>,
             spsl::StringBase<spsl::StorageArray<wchar_t, 64, spsl::policy::overflow::Truncate>>,
             spsl::StringBase<spsl::StorageArray<wchar_t, 64, spsl::policy::overflow::Throw>>,
             spsl::StringBase<spsl::StoragePassword<char>>,
             spsl::StringBase<spsl::StoragePassword<wchar_t>>,
             spsl::ArrayString<128, spsl::policy::overflow::Truncate>,
             spsl::ArrayStringW<128, spsl::policy::overflow::Truncate>,
             spsl::ArrayString<128, spsl::policy::overflow::Throw>,
             spsl::ArrayStringW<128, spsl::policy::overflow::Throw>, spsl::PasswordString,
             spsl::PasswordStringW>;

// printable/streamable types
using StringBaseStreamableTypes =
  std::tuple<spsl::StringBase<spsl::StorageArray<char, 64, spsl::policy::overflow::Truncate>>,
             spsl::StringBase<spsl::StorageArray<char, 64, spsl::policy::overflow::Throw>>,
             spsl::StringBase<spsl::StorageArray<wchar_t, 64, spsl::policy::overflow::Truncate>>,
             spsl::StringBase<spsl::StorageArray<wchar_t, 64, spsl::policy::overflow::Throw>>,
             spsl::ArrayString<128, spsl::policy::overflow::Truncate>,
             spsl::ArrayStringW<128, spsl::policy::overflow::Truncate>,
             spsl::ArrayString<128, spsl::policy::overflow::Throw>,
             spsl::ArrayStringW<128, spsl::policy::overflow::Throw>>;

// non-printable/streamable types
using StringBaseNonStreamableTypes = std::tuple<spsl::StringBase<spsl::StoragePassword<char>>,
                                                spsl::StringBase<spsl::StoragePassword<wchar_t>>,
                                                spsl::PasswordString, spsl::PasswordStringW>;

#endif /* TEST_TESTS_HPP_ */
