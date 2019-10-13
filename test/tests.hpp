/**
 * @file    Special Purpose Strings Library: tests.hpp
 * @author  Daniel Evers
 * @brief   Common test header
 * @license MIT
 */

#ifndef TEST_TESTS_HPP_
#define TEST_TESTS_HPP_

#include <tuple>

#include <gsl.hpp>
#include "catch.hpp"


// all string types we want to test (StringBase-specific only)
// (note: PasswordString[W] is not based on StringBase and therefore not included here)
using StringBaseTestTypes =
  std::tuple<spsl::StringBase<spsl::StorageArray<char, 64, spsl::policy::overflow::Throw>>,
             spsl::StringBase<spsl::StorageArray<wchar_t, 64, spsl::policy::overflow::Truncate>>,
             spsl::StringBase<spsl::StorageArray<wchar_t, 64, spsl::policy::overflow::Throw>>,
             spsl::StringBase<spsl::StoragePassword<char, 32>>,
             spsl::StringBase<spsl::StoragePassword<wchar_t, 32>>,
             spsl::ArrayString<128, spsl::policy::overflow::Truncate>,
             spsl::ArrayStringW<128, spsl::policy::overflow::Truncate>,
             spsl::ArrayString<128, spsl::policy::overflow::Throw>,
             spsl::ArrayStringW<128, spsl::policy::overflow::Throw>,
             // BYTE tests (using GSL)
             spsl::StringBase<spsl::StorageArray<gsl::byte, 64, spsl::policy::overflow::Truncate>>,
             spsl::StringBase<spsl::StoragePassword<gsl::byte, 32>>>;


// all string types we want to test: we already include StringBase here to avoid copy & paste
using StringCoreTestTypes = std::tuple<
  spsl::StringCore<spsl::StorageArray<char, 64, spsl::policy::overflow::Truncate>>,
  spsl::StringCore<spsl::StorageArray<char, 64, spsl::policy::overflow::Throw>>,
  spsl::StringCore<spsl::StorageArray<wchar_t, 64, spsl::policy::overflow::Truncate>>,
  spsl::StringCore<spsl::StorageArray<wchar_t, 64, spsl::policy::overflow::Throw>>,
  spsl::StringCore<spsl::StoragePassword<char>>, spsl::StringCore<spsl::StoragePassword<wchar_t>>,
  spsl::StringCore<spsl::StorageArray<char, 64, spsl::policy::overflow::Truncate>>,
  spsl::StringBase<spsl::StorageArray<char, 64, spsl::policy::overflow::Throw>>,
  spsl::StringBase<spsl::StorageArray<wchar_t, 64, spsl::policy::overflow::Truncate>>,
  spsl::StringBase<spsl::StorageArray<wchar_t, 64, spsl::policy::overflow::Throw>>,
  spsl::StringBase<spsl::StoragePassword<char>>, spsl::StringBase<spsl::StoragePassword<wchar_t>>,
  spsl::ArrayString<128, spsl::policy::overflow::Truncate>,
  spsl::ArrayStringW<128, spsl::policy::overflow::Truncate>,
  spsl::ArrayString<128, spsl::policy::overflow::Throw>,
  spsl::ArrayStringW<128, spsl::policy::overflow::Throw>, spsl::PasswordString,
  spsl::PasswordStringW,
  // BYTE tests (using GSL)
  spsl::StringCore<spsl::StorageArray<gsl::byte, 64, spsl::policy::overflow::Truncate>>,
  spsl::StringBase<spsl::StoragePassword<gsl::byte>>>;


#endif /* TEST_TESTS_HPP_ */
