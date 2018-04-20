/**
 * @file	Special Purpose Strings Library: test_stringbase.cpp
 * @author	Daniel Evers
 * @brief	StringCore unit tests
 * @license MIT
 */

#include <gtest/gtest.h>
#include <sstream>

#include "spsl.hpp"
#include "testdata.hpp"

template <typename CharType>
class StringBaseTest : public ::testing::Test
{
};

// all string types we want to test (StringBase-specific only)
// (note: PasswordString[W] is not based on StringBase and therefore not included here)
using SpecificTypes = testing::Types<
  spsl::StringBase<spsl::StorageArray<char, 64, spsl::policy::overflow::Throw>>,
  spsl::StringBase<spsl::StorageArray<wchar_t, 64, spsl::policy::overflow::Truncate>>,
  spsl::StringBase<spsl::StorageArray<wchar_t, 64, spsl::policy::overflow::Throw>>,
  spsl::StringBase<spsl::StoragePassword<char, 32>>,
  spsl::StringBase<spsl::StoragePassword<wchar_t, 32>>,
  spsl::ArrayString<128, spsl::policy::overflow::Truncate>,
  spsl::ArrayStringW<128, spsl::policy::overflow::Truncate>,
  spsl::ArrayString<128, spsl::policy::overflow::Throw>,
  spsl::ArrayStringW<128, spsl::policy::overflow::Throw>>;


TYPED_TEST_CASE(StringBaseTest, SpecificTypes);


/* find_first_of functions */
TYPED_TEST(StringBaseTest, FindFirstOfFunctions)
{
    using StringType = TypeParam; // gtest specific
    using StorageType = typename StringType::storage_type;
    using CharType = typename StorageType::char_type;
    const TestData<CharType> data{};
    const auto npos = StringType::npos;
    const auto nul = StringType::nul;

    const StringType s(data.hello_world);
    StringType sWithNul(s);
    sWithNul += nul;
    ASSERT_EQ(s.size() + 1, sWithNul.size());
    const CharType H = data.hello_world[0];
    const CharType l = data.hello_world[3];
    const CharType W = data.hello_world[6];
    const CharType b = data.blablabla[0];
    const std::basic_string<CharType> chars1{ H, l, W, b };
    const std::basic_string<CharType> chars2{ W, b };
    const std::basic_string<CharType> chars3{ b };
    const std::basic_string<CharType> chars4{ b, nul };

    // s = "Hello World!"
    //      0     6    11

    // find_first_of(const char_type* s, size_type pos, size_type count) const;
    ASSERT_EQ(s.find_first_of(data.empty, 0, 0), npos);
    ASSERT_EQ(s.find_first_of(chars1.data(), 0, chars1.size()), 0);
    ASSERT_EQ(s.find_first_of(chars2.data(), 0, chars2.size()), 6);
    ASSERT_EQ(s.find_first_of(chars2.data(), 7, chars2.size()), npos);
    ASSERT_EQ(s.find_first_of(chars3.data(), 0, chars3.size()), npos);
    ASSERT_EQ(s.find_first_of(chars4.data(), 0, chars4.size()), npos);
    ASSERT_EQ(sWithNul.find_first_of(chars4.data(), 0, chars4.size()), sWithNul.size() - 1);
    // pos >= size() is safe
    ASSERT_EQ(s.find_first_of(chars1.data(), s.size(), chars1.size()), npos);
    ASSERT_EQ(s.find_first_of(chars1.data(), s.size() + 1024, chars1.size()), npos);

    // find_first_of(const char_type* s, size_type pos = 0) const;
    ASSERT_EQ(s.find_first_of(chars1.data()), 0);
    ASSERT_EQ(s.find_first_of(chars1.data(), 0), 0);
    ASSERT_EQ(s.find_first_of(chars1.data(), 1), 2);
    ASSERT_EQ(s.find_first_of(chars2.data()), 6);
    ASSERT_EQ(s.find_first_of(chars2.data(), 0), 6);
    ASSERT_EQ(s.find_first_of(chars2.data(), 6), 6);
    ASSERT_EQ(s.find_first_of(chars2.data(), 7), npos);
    ASSERT_EQ(s.find_first_of(chars3.data()), npos);
    ASSERT_EQ(s.find_first_of(chars4.data()), npos);
    ASSERT_EQ(sWithNul.find_first_of(chars4.data()), npos);
    // pos >= size() is safe
    ASSERT_EQ(s.find_first_of(chars1.data(), s.size()), npos);
    ASSERT_EQ(s.find_first_of(chars1.data(), s.size() + 1024), npos);

    // find_first_of(char_type ch, size_type pos = 0) const;
    ASSERT_EQ(s.find_first_of(H), 0);
    ASSERT_EQ(s.find_first_of(H, 0), 0);
    ASSERT_EQ(s.find_first_of(H, 1), npos);
    ASSERT_EQ(s.find_first_of(l), 2);
    ASSERT_EQ(s.find_first_of(l, 0), 2);
    ASSERT_EQ(s.find_first_of(l, 2), 2);
    ASSERT_EQ(s.find_first_of(l, 3), 3);
    ASSERT_EQ(s.find_first_of(l, 4), 9);
    ASSERT_EQ(s.find_first_of(b), npos);
    ASSERT_EQ(sWithNul.find_first_of(nul), sWithNul.size() - 1);

    // find_first_of(const this_type& str, size_type pos = 0) const;
    ASSERT_EQ(s.find_first_of(chars1), 0);
    ASSERT_EQ(s.find_first_of(chars1, 0), 0);
    ASSERT_EQ(s.find_first_of(chars2), 6);
    ASSERT_EQ(s.find_first_of(chars2, 0), 6);
    ASSERT_EQ(s.find_first_of(chars2, 7), npos);
    ASSERT_EQ(s.find_first_of(chars3), npos);
    ASSERT_EQ(s.find_first_of(chars3, 0), npos);
    ASSERT_EQ(s.find_first_of(chars4), npos);
    ASSERT_EQ(s.find_first_of(chars4, 0), npos);
    ASSERT_EQ(sWithNul.find_first_of(chars4), sWithNul.size() - 1);
    ASSERT_EQ(sWithNul.find_first_of(chars4, 0), sWithNul.size() - 1);
}


/* find_first_not_of functions */
TYPED_TEST(StringBaseTest, FindFirstNotOfFunctions)
{
    using StringType = TypeParam; // gtest specific
    using StorageType = typename StringType::storage_type;
    using CharType = typename StorageType::char_type;
    const TestData<CharType> data{};
    const auto npos = StringType::npos;
    const auto nul = StringType::nul;

    const StringType s(data.hello_world);
    const CharType H = data.hello_world[0];
    const CharType l = data.hello_world[3];
    const CharType W = data.hello_world[6];
    const CharType b = data.blablabla[0];
    const StringType sH(13, H);
    const std::basic_string<CharType> chars1{ H, l, W, b };
    const std::basic_string<CharType> chars2{ W, b };
    const std::basic_string<CharType> chars3{ b };
    const std::basic_string<CharType> chars4{ b, nul };

    // s = "Hello World!"
    //      0     6    11

    // find_first_not_of(const char_type* s, size_type pos, size_type count) const;
    ASSERT_EQ(s.find_first_not_of(data.empty, 0, 0), 0);
    ASSERT_EQ(s.find_first_not_of(s.data(), 0, s.size()), npos);
    ASSERT_EQ(s.find_first_not_of(chars1.data(), 0, chars1.size()), 1);
    ASSERT_EQ(s.find_first_not_of(chars1.data(), 1, chars1.size()), 1);
    ASSERT_EQ(s.find_first_not_of(chars1.data(), 2, chars1.size()), 4);
    ASSERT_EQ(s.find_first_not_of(chars2.data(), 0, chars2.size()), 0);
    ASSERT_EQ(s.find_first_not_of(chars3.data(), s.size() - 1, chars3.size()), s.size() - 1);
    // pos >= size() is safe
    ASSERT_EQ(s.find_first_not_of(chars1.data(), s.size(), chars1.size()), npos);
    ASSERT_EQ(s.find_first_not_of(chars1.data(), s.size() + 1024, chars1.size()), npos);
    ASSERT_EQ(s.find_first_not_of(chars4.data(), s.size(), chars4.size()), npos);

    // find_first_not_of(const char_type* s, size_type pos = 0) const;
    ASSERT_EQ(s.find_first_not_of(s.data()), npos);
    ASSERT_EQ(s.find_first_not_of(s.data(), 0), npos);
    ASSERT_EQ(s.find_first_not_of(chars1.data()), 1);
    ASSERT_EQ(s.find_first_not_of(chars1.data(), 0), 1);
    ASSERT_EQ(s.find_first_not_of(chars1.data(), 1), 1);
    ASSERT_EQ(s.find_first_not_of(chars1.data(), 2), 4);
    ASSERT_EQ(s.find_first_not_of(chars2.data()), 0);
    ASSERT_EQ(s.find_first_not_of(chars2.data(), 0), 0);
    ASSERT_EQ(s.find_first_not_of(chars3.data(), s.size() - 1), s.size() - 1);
    // pos >= size() is safe
    ASSERT_EQ(s.find_first_not_of(chars1.data(), s.size()), npos);
    ASSERT_EQ(s.find_first_not_of(chars1.data(), s.size() + 1024), npos);
    ASSERT_EQ(s.find_first_not_of(chars4.data(), s.size()), npos);

    // find_first_not_of(char_type ch, size_type pos = 0) const;
    ASSERT_EQ(s.find_first_not_of(H), 1);
    ASSERT_EQ(s.find_first_not_of(H, 0), 1);
    ASSERT_EQ(s.find_first_not_of(l), 0);
    ASSERT_EQ(s.find_first_not_of(l, 0), 0);
    ASSERT_EQ(s.find_first_not_of(l, 2), 4);
    ASSERT_EQ(sH.find_first_not_of(l), 0);
    ASSERT_EQ(sH.find_first_not_of(l, 0), 0);
    ASSERT_EQ(sH.find_first_not_of(l, 1), 1);
    ASSERT_EQ(sH.find_first_not_of(H), npos);
    ASSERT_EQ(sH.find_first_not_of(H, sH.size() - 1), npos);
    // pos >= size() is safe
    ASSERT_EQ(sH.find_first_not_of(l, sH.size()), npos);
    ASSERT_EQ(sH.find_first_not_of(H, sH.size()), npos);

    // find_first_not_of(const this_type& str, size_type pos = 0) const;
    ASSERT_EQ(s.find_first_not_of(s.data()), npos);
    ASSERT_EQ(s.find_first_not_of(s.data(), 0), npos);
    ASSERT_EQ(s.find_first_not_of(chars1.data()), 1);
    ASSERT_EQ(s.find_first_not_of(chars1.data(), 0), 1);
    ASSERT_EQ(s.find_first_not_of(chars1.data(), 1), 1);
    ASSERT_EQ(s.find_first_not_of(chars1.data(), 2), 4);
    ASSERT_EQ(s.find_first_not_of(chars2.data()), 0);
    ASSERT_EQ(s.find_first_not_of(chars2.data(), 0), 0);
    ASSERT_EQ(s.find_first_not_of(chars3.data(), s.size() - 1), s.size() - 1);
    // pos >= size() is safe
    ASSERT_EQ(s.find_first_not_of(chars1.data(), s.size()), npos);
    ASSERT_EQ(s.find_first_not_of(chars1.data(), s.size() + 1024), npos);
    ASSERT_EQ(s.find_first_not_of(chars4.data(), s.size()), npos);
}


/* find_last_of functions */
TYPED_TEST(StringBaseTest, FindLastOfFunctions)
{
    using StringType = TypeParam; // gtest specific
    using StorageType = typename StringType::storage_type;
    using CharType = typename StorageType::char_type;
    const TestData<CharType> data{};
    const auto npos = StringType::npos;
    const auto nul = StringType::nul;

    const StringType s(data.hello_world);
    StringType sWithNul(s);
    sWithNul += nul;
    ASSERT_EQ(s.size() + 1, sWithNul.size());
    const CharType H = data.hello_world[0];
    const CharType l = data.hello_world[3];
    const CharType W = data.hello_world[6];
    const CharType b = data.blablabla[0];
    const StringType sH(13, H);
    const std::basic_string<CharType> chars1{ H, l, W, b };
    const std::basic_string<CharType> chars2{ W, b };
    const std::basic_string<CharType> chars3{ b };
    const std::basic_string<CharType> chars4{ b, nul };
    const std::basic_string<CharType> empty;

    // s = "Hello World!"
    //      0     6    11

    // find_last_of(const char_type* s, size_type pos, size_type count) const;
    ASSERT_EQ(s.find_last_of(s.data(), npos, s.size()), s.size() - 1);
    ASSERT_EQ(s.find_last_of(chars1.data(), npos, chars1.size()), 9);
    ASSERT_EQ(s.find_last_of(chars1.data(), 9, chars1.size()), 9);
    ASSERT_EQ(s.find_last_of(chars1.data(), 8, chars1.size()), 6);
    ASSERT_EQ(s.find_last_of(empty.data(), npos, 0), npos);
    ASSERT_EQ(s.find_last_of(empty.data(), 0, 0), npos);
    ASSERT_EQ(s.find_last_of(chars2.data(), npos, chars2.size()), 6);
    ASSERT_EQ(s.find_last_of(chars3.data(), npos, chars3.size()), npos);
    ASSERT_EQ(s.find_last_of(chars4.data(), npos, chars4.size()), npos);
    ASSERT_EQ(sWithNul.find_last_of(chars4.data(), npos, chars4.size()), sWithNul.size() - 1);
    // "npos" isn't special, it's just a value >= size() - so any value is valid
    ASSERT_EQ(s.find_last_of(chars1.data(), s.size(), chars1.size()), 9);
    ASSERT_EQ(s.find_last_of(chars1.data(), s.size() + 47, chars1.size()), 9);
    ASSERT_EQ(s.find_last_of(chars1.data(), s.size() + 2048, chars1.size()), 9);

    // find_last_of(const char_type* s, size_type pos = npos) const;
    ASSERT_EQ(s.find_last_of(chars1.data()), 9);
    ASSERT_EQ(s.find_last_of(chars1.data(), npos), 9);
    ASSERT_EQ(s.find_last_of(chars1.data(), 9), 9);
    ASSERT_EQ(s.find_last_of(chars1.data(), 8), 6);
    ASSERT_EQ(s.find_last_of(empty.data()), npos);
    ASSERT_EQ(s.find_last_of(empty.data(), npos), npos);
    ASSERT_EQ(s.find_last_of(chars2.data()), 6);
    ASSERT_EQ(s.find_last_of(chars2.data(), npos), 6);
    ASSERT_EQ(s.find_last_of(chars3.data()), npos);
    ASSERT_EQ(s.find_last_of(chars3.data(), npos), npos);
    ASSERT_EQ(s.find_last_of(chars4.data()), npos);
    ASSERT_EQ(s.find_last_of(chars4.data(), npos), npos);
    // finding NUL doesn't work without length
    ASSERT_EQ(sWithNul.find_last_of(chars4.data()), npos);
    ASSERT_EQ(sWithNul.find_last_of(chars4.data(), npos), npos);

    // find_last_of(char_type ch, size_type pos = npos) const;
    ASSERT_EQ(s.find_last_of(s.back()), 11);
    ASSERT_EQ(s.find_last_of(s.back(), npos), 11);
    ASSERT_EQ(s.find_last_of(H), 0);
    ASSERT_EQ(s.find_last_of(H, npos), 0);
    ASSERT_EQ(s.find_last_of(l), 9);
    ASSERT_EQ(s.find_last_of(l, npos), 9);
    ASSERT_EQ(s.find_last_of(l, 9), 9);
    ASSERT_EQ(s.find_last_of(l, 8), 3);
    ASSERT_EQ(s.find_last_of(l, 3), 3);
    ASSERT_EQ(s.find_last_of(l, 2), 2);
    ASSERT_EQ(s.find_last_of(l, 1), npos);
    ASSERT_EQ(s.find_last_of(l, 0), npos);
    ASSERT_EQ(s.find_last_of(b), npos);
    ASSERT_EQ(s.find_last_of(b, npos), npos);
    ASSERT_EQ(sWithNul.find_last_of(nul, npos), sWithNul.size() - 1);
    ASSERT_EQ(sWithNul.find_last_of(nul, s.size() - 2), npos);

    // find_last_of(const this_type& str, size_type pos = npos) const;
    ASSERT_EQ(s.find_last_of(chars1), 9);
    ASSERT_EQ(s.find_last_of(chars1, npos), 9);
    ASSERT_EQ(s.find_last_of(chars1, 9), 9);
    ASSERT_EQ(s.find_last_of(chars1, 8), 6);
    ASSERT_EQ(s.find_last_of(empty), npos);
    ASSERT_EQ(s.find_last_of(empty, npos), npos);
    ASSERT_EQ(s.find_last_of(empty, 0), npos);
    ASSERT_EQ(s.find_last_of(chars2), 6);
    ASSERT_EQ(s.find_last_of(chars2, npos), 6);
    ASSERT_EQ(s.find_last_of(chars3), npos);
    ASSERT_EQ(s.find_last_of(chars3, npos), npos);
    ASSERT_EQ(s.find_last_of(chars4), npos);
    ASSERT_EQ(s.find_last_of(chars4, npos), npos);
    ASSERT_EQ(sWithNul.find_last_of(chars4), sWithNul.size() - 1);
    ASSERT_EQ(sWithNul.find_last_of(chars4, npos), sWithNul.size() - 1);
    // "npos" isn't special, it's just a value >= size() - so any value is valid
    ASSERT_EQ(s.find_last_of(chars1, s.size()), 9);
    ASSERT_EQ(s.find_last_of(chars1, s.size() + 47), 9);
    ASSERT_EQ(s.find_last_of(chars1, s.size() + 2048), 9);
}

/* find_last_not_of functions */
TYPED_TEST(StringBaseTest, FindLastNotOfFunctions)
{
    using StringType = TypeParam; // gtest specific
    using StorageType = typename StringType::storage_type;
    using CharType = typename StorageType::char_type;
    const TestData<CharType> data{};
    const auto npos = StringType::npos;
    const auto nul = StringType::nul;

    const StringType s(data.hello_world);
    StringType sWithNul(s);
    sWithNul += nul;
    ASSERT_EQ(s.size() + 1, sWithNul.size());
    const CharType H = data.hello_world[0];
    const CharType l = data.hello_world[3];
    const CharType W = data.hello_world[6];
    const CharType b = data.blablabla[0];
    const StringType sH(13, H);
    const std::basic_string<CharType> chars1{ H, l, W, b };
    const std::basic_string<CharType> chars2{ W, b };
    const std::basic_string<CharType> chars3{ b };
    const std::basic_string<CharType> chars4{ b, nul };
    const std::basic_string<CharType> empty;

    // s = "Hello World!"
    //      0     6    11

    // find_last_not_of(const char_type* s, size_type pos, size_type count) const;
    ASSERT_EQ(s.find_last_not_of(empty.data(), npos, 0), s.size() - 1);
    ASSERT_EQ(s.find_last_not_of(s.data(), npos, s.size()), npos);
    ASSERT_EQ(s.find_last_not_of(s.data(), 0, s.size()), npos);
    ASSERT_EQ(s.find_last_not_of(chars1.data(), npos, chars1.size()), 11);
    ASSERT_EQ(s.find_last_not_of(chars1.data(), 6, chars1.size()), 5);
    ASSERT_EQ(s.find_last_not_of(chars2.data(), 6, chars2.size()), 5);
    ASSERT_EQ(s.find_last_not_of(chars3.data(), npos, chars3.size()), 11);
    ASSERT_EQ(s.find_last_not_of(chars3.data(), 11, chars3.size()), 11);
    ASSERT_EQ(s.find_last_not_of(chars3.data(), 10, chars3.size()), 10);
    ASSERT_EQ(s.find_last_not_of(chars3.data(), 9, chars3.size()), 9);
    ASSERT_EQ(sWithNul.find_last_not_of(chars4.data(), npos, chars4.size()), sWithNul.size() - 2);
    // "npos" isn't special, it's just a value >= size() - so any value is valid
    ASSERT_EQ(s.find_last_not_of(chars1.data(), s.size(), chars1.size()), 11);
    ASSERT_EQ(s.find_last_not_of(chars1.data(), s.size() + 47, chars1.size()), 11);
    ASSERT_EQ(s.find_last_not_of(chars1.data(), s.size() + 2048, chars1.size()), 11);

    // find_last_not_of(const char_type* s, size_type pos = npos) const;
    ASSERT_EQ(s.find_last_not_of(empty.data()), s.size() - 1);
    ASSERT_EQ(s.find_last_not_of(empty.data(), npos), s.size() - 1);
    ASSERT_EQ(s.find_last_not_of(s.data()), npos);
    ASSERT_EQ(s.find_last_not_of(s.data(), npos), npos);
    ASSERT_EQ(s.find_last_not_of(s.data(), 0), npos);
    ASSERT_EQ(s.find_last_not_of(chars1.data()), 11);
    ASSERT_EQ(s.find_last_not_of(chars1.data(), npos), 11);
    ASSERT_EQ(s.find_last_not_of(chars1.data(), 6, chars1.size()), 5);
    ASSERT_EQ(s.find_last_not_of(chars2.data(), 6, chars2.size()), 5);
    ASSERT_EQ(s.find_last_not_of(chars3.data()), 11);
    ASSERT_EQ(s.find_last_not_of(chars3.data(), npos), 11);
    ASSERT_EQ(s.find_last_not_of(chars3.data(), 11), 11);
    ASSERT_EQ(s.find_last_not_of(chars3.data(), 10), 10);
    ASSERT_EQ(s.find_last_not_of(chars3.data(), 9), 9);
    // finding NUL doesn't work without length
    ASSERT_EQ(sWithNul.find_last_not_of(chars4.data(), npos), sWithNul.size() - 1);

    // find_last_not_of(char_type ch, size_type pos = npos) const;
    ASSERT_EQ(s.find_last_not_of(s.back()), 10);
    ASSERT_EQ(s.find_last_not_of(s.back(), npos), 10);
    ASSERT_EQ(s.find_last_not_of(H), 11);
    ASSERT_EQ(s.find_last_not_of(H, npos), 11);
    ASSERT_EQ(s.find_last_not_of(H, 0), npos);
    ASSERT_EQ(s.find_last_not_of(W, 6), 5);
    ASSERT_EQ(s.find_last_not_of(l), 11);
    ASSERT_EQ(s.find_last_not_of(l, npos), 11);
    ASSERT_EQ(s.find_last_not_of(l, 9), 8);
    ASSERT_EQ(sWithNul.find_last_not_of(b), sWithNul.size() - 1);
    ASSERT_EQ(sWithNul.find_last_not_of(nul), sWithNul.size() - 2);

    // find_last_not_of(const this_type& str, size_type pos = npos) const;
    ASSERT_EQ(s.find_last_not_of(empty), s.size() - 1);
    ASSERT_EQ(s.find_last_not_of(empty, npos), s.size() - 1);
    ASSERT_EQ(s.find_last_not_of(s), npos);
    ASSERT_EQ(s.find_last_not_of(s, npos), npos);
    ASSERT_EQ(s.find_last_not_of(s, 0), npos);
    ASSERT_EQ(s.find_last_not_of(chars1), 11);
    ASSERT_EQ(s.find_last_not_of(chars1, npos), 11);
    ASSERT_EQ(s.find_last_not_of(chars1, 6), 5);
    ASSERT_EQ(s.find_last_not_of(chars2, 6), 5);
    ASSERT_EQ(s.find_last_not_of(chars3), 11);
    ASSERT_EQ(s.find_last_not_of(chars3, npos), 11);
    ASSERT_EQ(s.find_last_not_of(chars3, 11), 11);
    ASSERT_EQ(s.find_last_not_of(chars3, 10), 10);
    ASSERT_EQ(s.find_last_not_of(chars3, 9), 9);
    ASSERT_EQ(sWithNul.find_last_not_of(chars4), sWithNul.size() - 2);
    ASSERT_EQ(sWithNul.find_last_not_of(chars4, npos), sWithNul.size() - 2);
    // "npos" isn't special, it's just a value >= size() - so any value is valid
    ASSERT_EQ(s.find_last_not_of(chars1, s.size()), 11);
    ASSERT_EQ(s.find_last_not_of(chars1, s.size() + 47), 11);
    ASSERT_EQ(s.find_last_not_of(chars1, s.size() + 2048), 11);
}

/* erase functions */
TYPED_TEST(StringBaseTest, EraseFunctions)
{
    using StringType = TypeParam; // gtest specific
    using StorageType = typename StringType::storage_type;
    using CharType = typename StorageType::char_type;
    const TestData<CharType> data{};
    using RefType = std::basic_string<CharType>;

    // by index
    {
        StringType s(data.hello_world);
        RefType ref(data.hello_world);
        s.erase(2, 3);
        ref.erase(2, 3);
        ASSERT_EQ(s, ref);

        // need to disambiguate since our iterators are actually plain pointers
        constexpr size_t pos = 0;
        s.erase(pos);
        ref.erase(0u);
        ASSERT_EQ(s, ref);

        // no-op
        s.erase(s.length());
        ASSERT_EQ(s, ref);

        ASSERT_THROW(s.erase(s.length() + 1), std::out_of_range);
    }

    // single iterator
    {
        StringType s(data.hello_world);
        RefType ref(data.hello_world);
        auto sResult = s.erase(s.begin());
        auto refResult = ref.erase(ref.begin());
        ASSERT_EQ(s, ref);
        ASSERT_EQ(sResult, s.begin());
        ASSERT_EQ(refResult, ref.begin());

        sResult = s.erase(s.cbegin() + 4);
        refResult = ref.erase(ref.cbegin() + 4);
        ASSERT_EQ(s, ref);
        ASSERT_EQ(sResult, s.begin() + 4);
        ASSERT_EQ(refResult, ref.begin() + 4);

        ASSERT_THROW(s.erase(s.end() + 1), std::out_of_range);
    }

    // iterator range
    {
        StringType s(data.hello_world);
        RefType ref(data.hello_world);
        auto sResult = s.erase(s.begin() + 1, s.begin() + 3);
        auto refResult = ref.erase(ref.begin() + 1, ref.begin() + 3);
        ASSERT_EQ(s, ref);
        ASSERT_EQ(sResult, s.begin() + 1);
        ASSERT_EQ(refResult, ref.begin() + 1);

        sResult = s.erase(s.begin(), s.end());
        ASSERT_TRUE(s.empty());
        ASSERT_EQ(sResult, s.begin());
        ASSERT_EQ(sResult, s.end());
    }
}

/*
 * The following class templates are a big workaround for std::initializer_list problems on
 * Windows / MSVC. Here, we cannot use an initializer_list declared somewhere else, because
 * copying this instance results in unpredictable data - probably a bug somewhere in MSVC or their
 * STL. To avoid this, we explicitly write down the initializer list, but this requires that we
 * write the test twice - once for char, once for wchar_t.
 * And this is how the cookie crumbles...
 */

template <typename CharType, typename StringType>
class TestReplaceInitializerList
{
};

template <typename StringType>
class TestReplaceInitializerList<char, StringType>
{
public:
    void run()
    {
        TestData<char> data;
        using Traits = std::char_traits<char>;

        StringType s1(data.hello_world);
        std::string s2(data.hello_world);
        const size_t initListSize = 4;

        // same length
        s1.replace(s1.cbegin(), s1.cbegin() + initListSize, { 'T', 'e', 's', 't' });
        s2.replace(s2.begin(), s2.begin() + initListSize, { 'T', 'e', 's', 't' });
        ASSERT_EQ(s1.size(), s2.size());
        ASSERT_TRUE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);

        // longer (forcing a reallocation for Password, but shorter than the max. array)
        s1.replace(s1.cbegin(), s1.cbegin() + 1, { 'T', 'e', 's', 't' });
        s2.replace(s2.begin(), s2.begin() + 1, { 'T', 'e', 's', 't' });
        ASSERT_EQ(s1.size(), s2.size());
        ASSERT_TRUE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);

        // shorter
        s1.replace(s1.cbegin(), s1.cbegin() + initListSize + 3, { 'T', 'e', 's', 't' });
        s2.replace(s2.begin(), s2.begin() + initListSize + 3, { 'T', 'e', 's', 't' });
        ASSERT_EQ(s1.size(), s2.size());
        ASSERT_TRUE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);

        // with offset
        s1.replace(s1.begin() + 3, s1.begin() + 5, { 'T', 'e', 's', 't' });
        s2.replace(s2.begin() + 3, s2.begin() + 5, { 'T', 'e', 's', 't' });
        ASSERT_EQ(s1.size(), s2.size());
        ASSERT_TRUE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);
    }
};

template <typename StringType>
class TestReplaceInitializerList<wchar_t, StringType>
{
public:
    void run()
    {
        TestData<wchar_t> data;
        using Traits = std::char_traits<wchar_t>;

        StringType s1(data.hello_world);
        std::wstring s2(data.hello_world);
        const size_t initListSize = 4;

        // same length
        s1.replace(s1.cbegin(), s1.cbegin() + initListSize, { L'T', L'e', L's', L't' });
        s2.replace(s2.begin(), s2.begin() + initListSize, { L'T', L'e', L's', L't' });
        ASSERT_EQ(s1.size(), s2.size());
        ASSERT_TRUE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);

        // longer (forcing a reallocation for Password, but shorter than the max. array)
        s1.replace(s1.cbegin(), s1.cbegin() + 1, { L'T', L'e', L's', L't' });
        s2.replace(s2.begin(), s2.begin() + 1, { L'T', L'e', L's', L't' });
        ASSERT_EQ(s1.size(), s2.size());
        ASSERT_TRUE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);

        // shorter
        s1.replace(s1.cbegin(), s1.cbegin() + initListSize + 3, { L'T', L'e', L's', L't' });
        s2.replace(s2.begin(), s2.begin() + initListSize + 3, { L'T', L'e', L's', L't' });
        ASSERT_EQ(s1.size(), s2.size());
        ASSERT_TRUE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);

        // with offset
        s1.replace(s1.begin() + 3, s1.begin() + 5, { L'T', L'e', L's', L't' });
        s2.replace(s2.begin() + 3, s2.begin() + 5, { L'T', L'e', L's', L't' });
        ASSERT_EQ(s1.size(), s2.size());
        ASSERT_TRUE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);
    }
};

/* replace functions */
TYPED_TEST(StringBaseTest, ReplaceFunctions)
{
    using StringType = TypeParam; // gtest specific
    using StorageType = typename StringType::storage_type;
    using CharType = typename StorageType::char_type;
    using Traits = typename StorageType::traits_type;
    const TestData<CharType> data{};
    const auto npos = StringType::npos;

    using RefType = std::basic_string<CharType>;

    // replace(size_type pos, size_type count, const this_type& str);
    {
        StringType s1(data.hello_world);
        RefType s2(data.hello_world);
        StringType repl1;
        RefType repl2;

        // same length
        repl1.assign(data.blablabla, 3);
        repl2.assign(data.blablabla, 3);
        s1.replace(0, 3, repl1);
        s2.replace(0, 3, repl2);
        ASSERT_EQ(s1.size(), s2.size());
        ASSERT_TRUE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);

        // longer (forcing a reallocation for Password, but shorter than the max. array)
        repl1.assign(32, data.blablabla[0]);
        repl2.assign(32, data.blablabla[0]);
        s1.replace(0, 3, repl1);
        s2.replace(0, 3, repl2);
        ASSERT_EQ(s1.size(), s2.size());
        ASSERT_TRUE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);

        // shorter
        repl1.assign(1, data.blablabla[0]);
        repl2.assign(1, data.blablabla[0]);
        s1.replace(0, 3, repl1);
        s2.replace(0, 3, repl2);
        ASSERT_EQ(s1.size(), s2.size());
        ASSERT_TRUE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);

        // replace all
        repl1.clear();
        repl2.clear();
        s1.replace(0, s1.size(), repl1);
        s2.replace(0, s2.size(), repl2);
        ASSERT_EQ(s1.size(), s2.size());
        ASSERT_TRUE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);
        ASSERT_TRUE(s1.empty());
        ASSERT_TRUE(s2.empty());

        // exceptions
        // (1) invalid pos raises out_of_range
        ASSERT_THROW(s1.replace(s1.size(), s1.size() + 3, repl1), std::out_of_range);
        // (2) in our class, an invalid count does the same
        ASSERT_THROW(s1.replace(0, s1.size() + 1, repl1), std::out_of_range);
    }

    // replace(const_iterator first, const_iterator last, const this_type& str);
    {
        StringType s1(data.hello_world);
        RefType s2(data.hello_world);
        StringType repl1;
        RefType repl2;

        // same length
        repl1.assign(data.blablabla, 3);
        repl2.assign(data.blablabla, 3);
        s1.replace(s1.cbegin(), s1.cbegin() + 3, repl1);
        s2.replace(s2.begin(), s2.begin() + 3, repl2);
        ASSERT_EQ(s1.size(), s2.size());
        ASSERT_TRUE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);

        // longer (forcing a reallocation for Password, but shorter than the max. array)
        repl1.assign(32, data.blablabla[0]);
        repl2.assign(32, data.blablabla[0]);
        s1.replace(s1.cbegin(), s1.cbegin() + 3, repl1);
        s2.replace(s2.begin(), s2.begin() + 3, repl2);
        ASSERT_EQ(s1.size(), s2.size());
        ASSERT_TRUE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);

        // shorter
        repl1.assign(1, data.blablabla[0]);
        repl2.assign(1, data.blablabla[0]);
        s1.replace(s1.cbegin(), s1.cbegin() + 3, repl1);
        s2.replace(s2.begin(), s2.begin() + 3, repl2);
        ASSERT_EQ(s1.size(), s2.size());
        ASSERT_TRUE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);

        // replace all
        repl1.clear();
        repl2.clear();
        s1.replace(s1.cbegin(), s1.cend(), repl1);
        s2.replace(s2.begin(), s2.end(), repl2);
        ASSERT_EQ(s1.size(), s2.size());
        ASSERT_TRUE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);
        ASSERT_TRUE(s1.empty());
        ASSERT_TRUE(s2.empty());
    }

    // replace(size_type pos, size_type count, const this_type& str, size_type pos2, size_type
    // count2 = npos);
    {
        StringType repl1;
        RefType repl2;

        // same length
        repl1.assign(data.blablabla, 3);
        repl2.assign(data.blablabla, 3);
        std::array<size_t, 2> sizes1{ 3, npos };
        for (auto len : sizes1)
        {
            StringType s1(data.hello_world);
            RefType s2(data.hello_world);
            s1.replace(0, 3, repl1, 0, len);
            s2.replace(0, 3, repl2, 0, len);
            ASSERT_EQ(s1.size(), s2.size());
            ASSERT_TRUE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);
        }

        // longer (forcing a reallocation for Password, but shorter than the max. array)
        repl1.assign(32, data.blablabla[0]);
        repl2.assign(32, data.blablabla[0]);
        std::array<size_t, 2> sizes2{ 32, npos };
        for (auto len : sizes2)
        {
            StringType s1(data.hello_world);
            RefType s2(data.hello_world);
            s1.replace(0, 3, repl1, 0, len);
            s2.replace(0, 3, repl2, 0, len);
            ASSERT_EQ(s1.size(), s2.size());
            ASSERT_TRUE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);
        }

        // shorter
        repl1.assign(1, data.blablabla[0]);
        repl2.assign(1, data.blablabla[0]);
        std::array<size_t, 2> sizes3{ 1, npos };
        for (auto len : sizes3)
        {
            StringType s1(data.hello_world);
            RefType s2(data.hello_world);
            s1.replace(0, 3, repl1, 0, len);
            s2.replace(0, 3, repl2, 0, len);
            ASSERT_EQ(s1.size(), s2.size());
            ASSERT_TRUE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);
        }

        // various lengths, but with pos2 != 0
        repl1 = data.blablabla;
        repl2 = data.blablabla;
        std::array<size_t, 2> sizes4{ 3, npos };
        for (auto len : sizes4)
        {
            StringType s1(data.hello_world);
            RefType s2(data.hello_world);
            s1.replace(0, 3, repl1, 3, len);
            s2.replace(0, 3, repl2, 3, len);
            ASSERT_EQ(s1.size(), s2.size());
            ASSERT_TRUE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);
        }

        // replace all
        repl1.clear();
        repl2.clear();
        std::array<size_t, 2> sizes5{ 0, npos };
        for (auto len : sizes5)
        {
            StringType s1(data.hello_world);
            RefType s2(data.hello_world);
            s1.replace(0, s1.size(), repl1, 0, len);
            s2.replace(0, s2.size(), repl2, 0, len);
            ASSERT_EQ(s1.size(), s2.size());
            ASSERT_TRUE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);
            ASSERT_TRUE(s1.empty());
            ASSERT_TRUE(s2.empty());
        }

        // exceptions: verify that they are thrown and that the string doesn't change
        {
            StringType s1(data.hello_world);
            // (1) invalid pos raises out_of_range
            ASSERT_THROW(s1.replace(s1.size(), s1.size() + 3, repl1, 0, repl1.size()),
                         std::out_of_range);
            ASSERT_TRUE(s1 == data.hello_world);
            // (2) in our class, an invalid count does the same
            ASSERT_THROW(s1.replace(0, s1.size() + 1, repl1, 0, repl1.size()), std::out_of_range);
            ASSERT_TRUE(s1 == data.hello_world);
        }
    }

    // replace(const_iterator first, const_iterator last, InputIt first2, InputIt last2);
    {
        StringType s1(data.hello_world);
        RefType s2(data.hello_world);
        StringType repl1;
        RefType repl2;

        // same length
        repl1.assign(data.blablabla, 3);
        repl2.assign(data.blablabla, 3);
        s1.replace(s1.cbegin(), s1.cbegin() + 3, repl1.begin(), repl1.end());
        s2.replace(s2.begin(), s2.begin() + 3, repl2.begin(), repl2.end());
        ASSERT_EQ(s1.size(), s2.size());
        ASSERT_TRUE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);
        // reverse_iterator
        s1.replace(s1.cbegin(), s1.cbegin() + 3, repl1.crbegin(), repl1.crend());
        s2.replace(s2.begin(), s2.begin() + 3, repl2.crbegin(), repl2.crend());
        ASSERT_EQ(s1.size(), s2.size());
        ASSERT_TRUE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);

        // longer (forcing a reallocation for Password, but shorter than the max. array)
        repl1.assign(32, data.blablabla[0]);
        repl2.assign(32, data.blablabla[0]);
        s1.replace(s1.cbegin(), s1.cbegin() + 3, repl1.begin(), repl1.end());
        s2.replace(s2.begin(), s2.begin() + 3, repl2.begin(), repl2.end());
        ASSERT_EQ(s1.size(), s2.size());
        ASSERT_TRUE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);
        // again, using std::vector for our string class
        std::vector<CharType> vec(32, data.blablabla[0]);
        s1 = data.hello_world;
        s2 = data.hello_world;
        s1.replace(s1.cbegin(), s1.cbegin() + 3, vec.begin(), vec.end());
        s2.replace(s2.begin(), s2.begin() + 3, repl2.begin(), repl2.end());
        ASSERT_EQ(s1.size(), s2.size());
        ASSERT_TRUE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);

        // shorter
        repl1.assign(1, data.blablabla[0]);
        repl2.assign(1, data.blablabla[0]);
        s1.replace(s1.cbegin(), s1.cbegin() + 3, repl1.begin(), repl1.end());
        s2.replace(s2.begin(), s2.begin() + 3, repl2.begin(), repl2.end());
        ASSERT_EQ(s1.size(), s2.size());
        ASSERT_TRUE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);

        // replace all
        repl1.clear();
        repl2.clear();
        s1.replace(s1.begin(), s1.end(), repl1.begin(), repl1.end());
        s2.replace(s2.begin(), s2.end(), repl2.begin(), repl2.end());
        ASSERT_EQ(s1.size(), s2.size());
        ASSERT_TRUE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);
        ASSERT_TRUE(s1.empty());
        ASSERT_TRUE(s2.empty());
    }

    // replace(size_type pos, size_type count, const char_type* cstr, size_type count2);
    {
        StringType repl1;
        RefType repl2;

        // same length
        repl1.assign(data.blablabla, 3);
        repl2.assign(data.blablabla, 3);
        std::array<size_t, 1> sizes1{ 3 };
        for (auto len : sizes1)
        {
            StringType s1(data.hello_world);
            RefType s2(data.hello_world);
            s1.replace(0, 3, repl1.data(), len);
            s2.replace(0, 3, repl2.data(), len);
            ASSERT_EQ(s1.size(), s2.size());
            ASSERT_TRUE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);
        }

        // longer (forcing a reallocation for Password, but shorter than the max. array)
        repl1.assign(32, data.blablabla[0]);
        repl2.assign(32, data.blablabla[0]);
        std::array<size_t, 2> sizes2{ 13, 32 };
        for (auto len : sizes2)
        {
            StringType s1(data.hello_world);
            RefType s2(data.hello_world);
            s1.replace(0, 3, repl1.data(), len);
            s2.replace(0, 3, repl2.data(), len);
            ASSERT_EQ(s1.size(), s2.size());
            ASSERT_TRUE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);
        }

        // shorter
        repl1.assign(1, data.blablabla[0]);
        repl2.assign(1, data.blablabla[0]);
        std::array<size_t, 1> sizes3{ 1 };
        for (auto len : sizes3)
        {
            StringType s1(data.hello_world);
            RefType s2(data.hello_world);
            s1.replace(0, 3, repl1.data(), len);
            s2.replace(0, 3, repl2.data(), len);
            ASSERT_EQ(s1.size(), s2.size());
            ASSERT_TRUE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);
        }

        // various lengths, but with offset
        repl1 = data.blablabla;
        repl2 = data.blablabla;
        std::array<size_t, 2> sizes4{ 3, data.blablabla_len };
        for (auto len : sizes4)
        {
            StringType s1(data.hello_world);
            RefType s2(data.hello_world);
            s1.replace(1, 3, repl1.data() + 3, len);
            s2.replace(1, 3, repl2.data() + 3, len);
            ASSERT_EQ(s1.size(), s2.size());
            ASSERT_TRUE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);
        }

        // replace all
        repl1.clear();
        repl2.clear();
        std::array<size_t, 1> sizes5{ 0 };
        for (auto len : sizes5)
        {
            StringType s1(data.hello_world);
            RefType s2(data.hello_world);
            s1.replace(0, s1.size(), repl1.data(), len);
            s2.replace(0, s2.size(), repl2.data(), len);
            ASSERT_EQ(s1.size(), s2.size());
            ASSERT_TRUE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);
            ASSERT_TRUE(s1.empty());
            ASSERT_TRUE(s2.empty());
        }

        // exceptions: verify that they are thrown and that the string doesn't change
        {
            StringType s1(data.hello_world);
            // (1) invalid pos raises out_of_range
            ASSERT_THROW(s1.replace(s1.size(), s1.size() + 3, repl1.data(), repl1.size()),
                         std::out_of_range);
            ASSERT_TRUE(s1 == data.hello_world);
            // (2) in our class, an invalid count does the same
            ASSERT_THROW(s1.replace(0, s1.size() + 1, repl1.data(), repl1.size()),
                         std::out_of_range);
            ASSERT_TRUE(s1 == data.hello_world);
        }
    }

    // replace(const_iterator first, const_iterator last, const char_type* cstr, size_type count2);
    {
        StringType repl1;
        RefType repl2;

        // same length
        repl1.assign(data.blablabla, 3);
        repl2.assign(data.blablabla, 3);
        std::array<size_t, 1> sizes1{ 3 };
        for (auto len : sizes1)
        {
            StringType s1(data.hello_world);
            RefType s2(data.hello_world);
            s1.replace(s1.cbegin(), s1.cbegin() + 3, repl1.data(), len);
            s2.replace(s2.begin(), s2.begin() + 3, repl2.data(), len);
            ASSERT_EQ(s1.size(), s2.size());
            ASSERT_TRUE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);
        }

        // longer (forcing a reallocation for Password, but shorter than the max. array)
        repl1.assign(32, data.blablabla[0]);
        repl2.assign(32, data.blablabla[0]);
        std::array<size_t, 2> sizes2{ 13, 32 };
        for (auto len : sizes2)
        {
            StringType s1(data.hello_world);
            RefType s2(data.hello_world);
            s1.replace(s1.cbegin(), s1.cbegin() + 3, repl1.data(), len);
            s2.replace(s2.begin(), s2.begin() + 3, repl2.data(), len);
            ASSERT_EQ(s1.size(), s2.size());
            ASSERT_TRUE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);
        }

        // shorter
        repl1.assign(1, data.blablabla[0]);
        repl2.assign(1, data.blablabla[0]);
        std::array<size_t, 1> sizes3{ 1 };
        for (auto len : sizes3)
        {
            StringType s1(data.hello_world);
            RefType s2(data.hello_world);
            s1.replace(s1.cbegin(), s1.cbegin() + 3, repl1.data(), len);
            s2.replace(s2.begin(), s2.begin() + 3, repl2.data(), len);
            ASSERT_EQ(s1.size(), s2.size());
            ASSERT_TRUE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);
        }

        // various lengths, but with offsets
        repl1 = data.blablabla;
        repl2 = data.blablabla;
        std::array<size_t, 2> sizes4{ 3, data.blablabla_len };
        for (auto len : sizes4)
        {
            StringType s1(data.hello_world);
            RefType s2(data.hello_world);
            s1.replace(s1.cbegin() + 1, s1.cbegin() + 5, repl1.data() + 3, len);
            s2.replace(s2.begin() + 1, s2.begin() + 5, repl2.data() + 3, len);
            ASSERT_EQ(s1.size(), s2.size());
            ASSERT_TRUE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);
        }

        // replace all
        repl1.clear();
        repl2.clear();
        std::array<size_t, 1> sizes5{ 0 };
        for (auto len : sizes5)
        {
            StringType s1(data.hello_world);
            RefType s2(data.hello_world);
            s1.replace(s1.begin(), s1.end(), repl1.data(), len);
            s2.replace(s2.begin(), s2.end(), repl2.data(), len);
            ASSERT_EQ(s1.size(), s2.size());
            ASSERT_TRUE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);
            ASSERT_TRUE(s1.empty());
            ASSERT_TRUE(s2.empty());
        }
    }

    // replace(size_type pos, size_type count, const char_type* cstr);
    {
        StringType s1(data.hello_world);
        RefType s2(data.hello_world);
        StringType repl1;
        RefType repl2;

        // same length
        repl1.assign(data.blablabla, 3);
        repl2.assign(data.blablabla, 3);
        s1.replace(0, 3, repl1.data());
        s2.replace(0, 3, repl2.data());
        ASSERT_EQ(s1.size(), s2.size());
        ASSERT_TRUE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);

        // longer (forcing a reallocation for Password, but shorter than the max. array)
        repl1.assign(32, data.blablabla[0]);
        repl2.assign(32, data.blablabla[0]);
        s1.replace(0, 3, repl1.data());
        s2.replace(0, 3, repl2.data());
        ASSERT_EQ(s1.size(), s2.size());
        ASSERT_TRUE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);

        // shorter
        repl1.assign(1, data.blablabla[0]);
        repl2.assign(1, data.blablabla[0]);
        s1.replace(0, 3, repl1.data());
        s2.replace(0, 3, repl2.data());
        ASSERT_EQ(s1.size(), s2.size());
        ASSERT_TRUE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);

        // with offset
        repl1 = data.blablabla;
        repl2 = data.blablabla;
        s1 = data.hello_world;
        s2 = data.hello_world;
        s1.replace(3, 7, repl1.data());
        s2.replace(3, 7, repl2.data());
        ASSERT_EQ(s1.size(), s2.size());
        ASSERT_TRUE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);

        // replace all
        repl1.clear();
        repl2.clear();
        s1.replace(0, s1.size(), repl1.data());
        s2.replace(0, s2.size(), repl2.data());
        ASSERT_EQ(s1.size(), s2.size());
        ASSERT_TRUE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);
        ASSERT_TRUE(s1.empty());
        ASSERT_TRUE(s2.empty());

        // exceptions: verify that they are thrown and that the string doesn't change
        {
            StringType s3(data.hello_world);
            // (1) invalid pos raises out_of_range
            ASSERT_THROW(s3.replace(s3.size(), s3.size() + 3, repl1.data()), std::out_of_range);
            ASSERT_TRUE(s3 == data.hello_world);
            // (2) in our class, an invalid count does the same
            ASSERT_THROW(s3.replace(0, s3.size() + 1, repl1.data()), std::out_of_range);
            ASSERT_TRUE(s3 == data.hello_world);
        }
    }

    // replace(const_iterator first, const_iterator last, const char_type* cstr);
    {
        StringType s1(data.hello_world);
        RefType s2(data.hello_world);
        StringType repl1;
        RefType repl2;

        // same length
        repl1.assign(data.blablabla, 3);
        repl2.assign(data.blablabla, 3);
        s1.replace(s1.cbegin(), s1.cbegin() + 3, repl1.data());
        s2.replace(s2.begin(), s2.begin() + 3, repl2.data());
        ASSERT_EQ(s1.size(), s2.size());
        ASSERT_TRUE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);

        // longer (forcing a reallocation for Password, but shorter than the max. array)
        repl1.assign(32, data.blablabla[0]);
        repl2.assign(32, data.blablabla[0]);
        s1.replace(s1.cbegin(), s1.cbegin() + 3, repl1.data());
        s2.replace(s2.begin(), s2.begin() + 3, repl2.data());
        ASSERT_EQ(s1.size(), s2.size());
        ASSERT_TRUE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);

        // shorter
        repl1.assign(1, data.blablabla[0]);
        repl2.assign(1, data.blablabla[0]);
        s1.replace(s1.cbegin(), s1.cbegin() + 3, repl1.data());
        s2.replace(s2.begin(), s2.begin() + 3, repl2.data());
        ASSERT_EQ(s1.size(), s2.size());
        ASSERT_TRUE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);

        // with offset
        repl1 = data.blablabla;
        repl2 = data.blablabla;
        s1 = data.hello_world;
        s2 = data.hello_world;
        s1.replace(s1.begin() + 3, s1.begin() + 7, repl1.data());
        s2.replace(s2.begin() + 3, s2.begin() + 7, repl2.data());
        ASSERT_EQ(s1.size(), s2.size());
        ASSERT_TRUE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);

        // replace all
        repl1.clear();
        repl2.clear();
        s1.replace(s1.begin(), s1.end(), repl1.data());
        s2.replace(s2.begin(), s2.end(), repl2.data());
        ASSERT_EQ(s1.size(), s2.size());
        ASSERT_TRUE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);
        ASSERT_TRUE(s1.empty());
        ASSERT_TRUE(s2.empty());
    }

    // replace(size_type pos, size_type count, size_type count2, char_type ch);
    {
        StringType s1(data.hello_world);
        RefType s2(data.hello_world);
        const CharType repl = data.blablabla[1];

        // same length
        s1.replace(0, 3, 3, repl);
        s2.replace(0, 3, 3, repl);
        ASSERT_EQ(s1.size(), s2.size());
        ASSERT_TRUE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);

        // longer (forcing a reallocation for Password, but shorter than the max. array)
        s1.replace(0, 3, 32, repl);
        s2.replace(0, 3, 32, repl);
        ASSERT_EQ(s1.size(), s2.size());
        ASSERT_TRUE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);

        // shorter
        s1.replace(0, 12, 1, repl);
        s2.replace(0, 12, 1, repl);
        ASSERT_EQ(s1.size(), s2.size());
        ASSERT_TRUE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);

        // various lengths, but with offset
        std::array<size_t, 5> sizes4{ 0, 1, 3, 5, data.blablabla_len };
        for (auto len : sizes4)
        {
            StringType s3(data.hello_world);
            RefType s4(data.hello_world);
            s3.replace(3, 4, len, repl);
            s4.replace(3, 4, len, repl);
            ASSERT_EQ(s3.size(), s4.size());
            ASSERT_TRUE(Traits::compare(s3.data(), s4.data(), s3.size()) == 0);
        }

        // replace all
        s1.replace(0, s1.size(), 0, repl);
        s2.replace(0, s2.size(), 0, repl);
        ASSERT_EQ(s1.size(), s2.size());
        ASSERT_TRUE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);
        ASSERT_TRUE(s1.empty());
        ASSERT_TRUE(s2.empty());

        // exceptions: verify that they are thrown and that the string doesn't change
        {
            s1 = data.hello_world;
            // (1) invalid pos raises out_of_range
            ASSERT_THROW(s1.replace(s1.size(), s1.size() + 3, 3, repl), std::out_of_range);
            ASSERT_TRUE(s1 == data.hello_world);
            // (2) in our class, an invalid count does the same
            ASSERT_THROW(s1.replace(0, s1.size() + 1, 3, repl), std::out_of_range);
            ASSERT_TRUE(s1 == data.hello_world);
        }
    }

    // replace(const_iterator first, const_iterator last, size_type count2, char_type ch);
    {
        StringType s1(data.hello_world);
        RefType s2(data.hello_world);
        const CharType repl = data.blablabla[1];

        // same length
        s1.replace(s1.cbegin(), s1.cbegin() + 3, 3, repl);
        s2.replace(s2.begin(), s2.begin() + 3, 3, repl);
        ASSERT_EQ(s1.size(), s2.size());
        ASSERT_TRUE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);

        // longer (forcing a reallocation for Password, but shorter than the max. array)
        s1.replace(s1.cbegin(), s1.cbegin() + 3, 32, repl);
        s2.replace(s2.begin(), s2.begin() + 3, 32, repl);
        ASSERT_EQ(s1.size(), s2.size());
        ASSERT_TRUE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);

        // shorter
        s1.replace(s1.cbegin(), s1.cbegin() + 12, 1, repl);
        s2.replace(s2.begin(), s2.begin() + 12, 1, repl);
        ASSERT_EQ(s1.size(), s2.size());
        ASSERT_TRUE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);

        // various lengths, but with offset
        std::array<size_t, 5> sizes4{ 0, 1, 3, 5, data.blablabla_len };
        for (auto len : sizes4)
        {
            StringType s3(data.hello_world);
            RefType s4(data.hello_world);
            s3.replace(s3.cbegin() + 3, s3.cbegin() + 4, len, repl);
            s4.replace(s4.begin() + 3, s4.begin() + 4, len, repl);
            ASSERT_EQ(s3.size(), s4.size());
            ASSERT_TRUE(Traits::compare(s3.data(), s4.data(), s3.size()) == 0);
        }

        // replace all
        s1.replace(s1.begin(), s1.end(), 0, repl);
        s2.replace(s2.begin(), s2.end(), 0, repl);
        ASSERT_EQ(s1.size(), s2.size());
        ASSERT_TRUE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);
        ASSERT_TRUE(s1.empty());
        ASSERT_TRUE(s2.empty());
    }

    // replace(const_iterator first, const_iterator last, std::initializer_list<char_type> ilist);
    {
        // to work around some strange MSVC issues with std::initializer_list, we avoid using
        // TestData
        TestReplaceInitializerList<CharType, StringType> t;
        t.run();
    }

    // replace(size_type pos, size_type count, const String&);
    {
        StringType s1(data.hello_world);
        RefType s2(data.hello_world);
        // we use basic_string here
        RefType repl;

        // same length
        repl.assign(data.blablabla, 3);
        s1.replace(0, 3, repl);
        s2.replace(0, 3, repl);
        ASSERT_EQ(s1.size(), s2.size());
        ASSERT_TRUE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);

        // longer (forcing a reallocation for Password, but shorter than the max. array)
        repl.assign(32, data.blablabla[0]);
        s1.replace(0, 3, repl);
        s2.replace(0, 3, repl);
        ASSERT_EQ(s1.size(), s2.size());
        ASSERT_TRUE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);

        // shorter
        repl.assign(1, data.blablabla[0]);
        s1.replace(1, 4, repl);
        s2.replace(1, 4, repl);
        ASSERT_EQ(s1.size(), s2.size());
        ASSERT_TRUE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);

        // replace all
        repl.clear();
        s1.replace(0, s1.size(), repl);
        s2.replace(0, s2.size(), repl);
        ASSERT_EQ(s1.size(), s2.size());
        ASSERT_TRUE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);
        ASSERT_TRUE(s1.empty());
        ASSERT_TRUE(s2.empty());

        // exceptions
        s1 = data.hello_world;
        repl = data.blablabla;
        // (1) invalid pos raises out_of_range
        ASSERT_THROW(s1.replace(s1.size(), s1.size() + 3, repl), std::out_of_range);
        // (2) in our class, an invalid count does the same
        ASSERT_THROW(s1.replace(0, s1.size() + 1, repl), std::out_of_range);
    }

    // replace(const_iterator first, const_iterator last, const String&);
    {
        StringType s1(data.hello_world);
        RefType s2(data.hello_world);
        // we use basic_string here
        RefType repl;

        // same length
        repl.assign(data.blablabla, 3);
        s1.replace(s1.cbegin(), s1.cbegin() + 3, repl);
        s2.replace(s2.begin(), s2.begin() + 3, repl);
        ASSERT_EQ(s1.size(), s2.size());
        ASSERT_TRUE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);

        // longer (forcing a reallocation for Password, but shorter than the max. array)
        repl.assign(32, data.blablabla[0]);
        s1.replace(s1.cbegin(), s1.cbegin() + 3, repl);
        s2.replace(s2.begin(), s2.begin() + 3, repl);
        ASSERT_EQ(s1.size(), s2.size());
        ASSERT_TRUE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);

        // shorter
        repl.assign(1, data.blablabla[0]);
        s1.replace(s1.cbegin() + 1, s1.cbegin() + 4, repl);
        s2.replace(s2.begin() + 1, s2.begin() + 4, repl);
        ASSERT_EQ(s1.size(), s2.size());
        ASSERT_TRUE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);

        // replace all
        repl.clear();
        s1.replace(s1.begin(), s1.end(), repl);
        s2.replace(s2.begin(), s2.end(), repl);
        ASSERT_EQ(s1.size(), s2.size());
        ASSERT_TRUE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);
        ASSERT_TRUE(s1.empty());
        ASSERT_TRUE(s2.empty());
    }

    // replace(size_type pos, size_type count, const String&, size_type pos2, size_type count2 =
    // npos);
    {
        // again, using basic_string
        RefType repl;

        // same length
        repl.assign(data.blablabla, 3);
        std::array<size_t, 2> sizes1{ 3, npos };
        for (auto len : sizes1)
        {
            StringType s1(data.hello_world);
            RefType s2(data.hello_world);
            s1.replace(0, 3, repl, 0, len);
            s2.replace(0, 3, repl, 0, len);
            ASSERT_EQ(s1.size(), s2.size());
            ASSERT_TRUE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);
        }

        // longer (forcing a reallocation for Password, but shorter than the max. array)
        repl.assign(32, data.blablabla[0]);
        std::array<size_t, 2> sizes2{ 32, npos };
        for (auto len : sizes2)
        {
            StringType s1(data.hello_world);
            RefType s2(data.hello_world);
            s1.replace(0, 3, repl, 0, len);
            s2.replace(0, 3, repl, 0, len);
            ASSERT_EQ(s1.size(), s2.size());
            ASSERT_TRUE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);
        }

        // shorter
        repl.assign(1, data.blablabla[0]);
        std::array<size_t, 2> sizes3{ 1, npos };
        for (auto len : sizes3)
        {
            StringType s1(data.hello_world);
            RefType s2(data.hello_world);
            s1.replace(0, 3, repl, 0, len);
            s2.replace(0, 3, repl, 0, len);
            ASSERT_EQ(s1.size(), s2.size());
            ASSERT_TRUE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);
        }

        // various lengths, but with pos2 != 0
        repl = data.blablabla;
        std::array<size_t, 2> sizes4{ 3, npos };
        for (auto len : sizes4)
        {
            StringType s1(data.hello_world);
            RefType s2(data.hello_world);
            s1.replace(0, 3, repl, 3, len);
            s2.replace(0, 3, repl, 3, len);
            ASSERT_EQ(s1.size(), s2.size());
            ASSERT_TRUE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);
        }

        // replace all
        repl.clear();
        std::array<size_t, 2> sizes5{ 0, npos };
        for (auto len : sizes5)
        {
            StringType s1(data.hello_world);
            RefType s2(data.hello_world);
            s1.replace(0, s1.size(), repl, 0, len);
            s2.replace(0, s2.size(), repl, 0, len);
            ASSERT_EQ(s1.size(), s2.size());
            ASSERT_TRUE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);
            ASSERT_TRUE(s1.empty());
            ASSERT_TRUE(s2.empty());
        }

        // exceptions: verify that they are thrown and that the string doesn't change
        {
            StringType s3(data.hello_world);
            // (1) invalid pos raises out_of_range
            ASSERT_THROW(s3.replace(s3.size(), s3.size() + 3, repl, 0, repl.size()),
                         std::out_of_range);
            ASSERT_TRUE(s3 == data.hello_world);
            // (2) in our class, an invalid count does the same
            ASSERT_THROW(s3.replace(0, s3.size() + 1, repl, 0, repl.size()), std::out_of_range);
            ASSERT_TRUE(s3 == data.hello_world);
        }
    }
}

/* operator<< */
TYPED_TEST(StringBaseTest, OutputStream)
{
    using StringType = TypeParam; // gtest specific
    using StorageType = typename StringType::storage_type;
    using CharType = typename StorageType::char_type;
    const TestData<CharType> data{};

    const StringType s(data.hello_world);
    const std::basic_string<CharType> ref(data.hello_world);

    // using a string stream for testing
    std::basic_stringstream<CharType> outputStream;
    outputStream << s;
    ASSERT_EQ(outputStream.str(), ref);
}
