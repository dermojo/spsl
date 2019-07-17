/**
 * @file	Special Purpose Strings Library: test_stringbase_find.cpp
 * @author	Daniel Evers
 * @brief	StringBase unit tests: find functions
 * @license MIT
 */

#include <gtest/gtest.h>
#include <sstream>

#include "spsl.hpp"
#include "testdata.hpp"
#include "tests.hpp"


/* find_first_of functions */
TYPED_TEST(StringBaseTest, FindFirstOfFunctions)
{
    using StringType = TypeParam; // gtest specific
    using StorageType = typename StringType::storage_type;
    using CharType = typename StorageType::char_type;
    const TestData<CharType> data;
    const auto npos = StringType::npos;
    const auto nul = StringType::nul();

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
    ASSERT_EQ(s.find_first_of(chars1.data(), 0, chars1.size()), 0u);
    ASSERT_EQ(s.find_first_of(chars2.data(), 0, chars2.size()), 6u);
    ASSERT_EQ(s.find_first_of(chars2.data(), 7, chars2.size()), npos);
    ASSERT_EQ(s.find_first_of(chars3.data(), 0, chars3.size()), npos);
    ASSERT_EQ(s.find_first_of(chars4.data(), 0, chars4.size()), npos);
    ASSERT_EQ(sWithNul.find_first_of(chars4.data(), 0, chars4.size()), sWithNul.size() - 1);
    // pos >= size() is safe
    ASSERT_EQ(s.find_first_of(chars1.data(), s.size(), chars1.size()), npos);
    ASSERT_EQ(s.find_first_of(chars1.data(), s.size() + 1024, chars1.size()), npos);

    // find_first_of(const char_type* s, size_type pos = 0) const;
    ASSERT_EQ(s.find_first_of(chars1.data()), 0u);
    ASSERT_EQ(s.find_first_of(chars1.data(), 0), 0u);
    ASSERT_EQ(s.find_first_of(chars1.data(), 1), 2u);
    ASSERT_EQ(s.find_first_of(chars2.data()), 6u);
    ASSERT_EQ(s.find_first_of(chars2.data(), 0), 6u);
    ASSERT_EQ(s.find_first_of(chars2.data(), 6), 6u);
    ASSERT_EQ(s.find_first_of(chars2.data(), 7), npos);
    ASSERT_EQ(s.find_first_of(chars3.data()), npos);
    ASSERT_EQ(s.find_first_of(chars4.data()), npos);
    ASSERT_EQ(sWithNul.find_first_of(chars4.data()), npos);
    // pos >= size() is safe
    ASSERT_EQ(s.find_first_of(chars1.data(), s.size()), npos);
    ASSERT_EQ(s.find_first_of(chars1.data(), s.size() + 1024), npos);

    // find_first_of(char_type ch, size_type pos = 0) const;
    ASSERT_EQ(s.find_first_of(H), 0u);
    ASSERT_EQ(s.find_first_of(H, 0), 0u);
    ASSERT_EQ(s.find_first_of(H, 1), npos);
    ASSERT_EQ(s.find_first_of(l), 2u);
    ASSERT_EQ(s.find_first_of(l, 0), 2u);
    ASSERT_EQ(s.find_first_of(l, 2), 2u);
    ASSERT_EQ(s.find_first_of(l, 3), 3u);
    ASSERT_EQ(s.find_first_of(l, 4), 9u);
    ASSERT_EQ(s.find_first_of(b), npos);
    ASSERT_EQ(sWithNul.find_first_of(nul), sWithNul.size() - 1);

    // find_first_of(const this_type& str, size_type pos = 0) const;
    ASSERT_EQ(s.find_first_of(chars1), 0u);
    ASSERT_EQ(s.find_first_of(chars1, 0), 0u);
    ASSERT_EQ(s.find_first_of(chars2), 6u);
    ASSERT_EQ(s.find_first_of(chars2, 0), 6u);
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
    const TestData<CharType> data;
    const auto npos = StringType::npos;
    const auto nul = StringType::nul();

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
    ASSERT_EQ(s.find_first_not_of(data.empty, 0, 0), 0u);
    ASSERT_EQ(s.find_first_not_of(s.data(), 0, s.size()), npos);
    ASSERT_EQ(s.find_first_not_of(chars1.data(), 0, chars1.size()), 1u);
    ASSERT_EQ(s.find_first_not_of(chars1.data(), 1, chars1.size()), 1u);
    ASSERT_EQ(s.find_first_not_of(chars1.data(), 2, chars1.size()), 4u);
    ASSERT_EQ(s.find_first_not_of(chars2.data(), 0, chars2.size()), 0u);
    ASSERT_EQ(s.find_first_not_of(chars3.data(), s.size() - 1, chars3.size()), s.size() - 1);
    // pos >= size() is safe
    ASSERT_EQ(s.find_first_not_of(chars1.data(), s.size(), chars1.size()), npos);
    ASSERT_EQ(s.find_first_not_of(chars1.data(), s.size() + 1024, chars1.size()), npos);
    ASSERT_EQ(s.find_first_not_of(chars4.data(), s.size(), chars4.size()), npos);

    // find_first_not_of(const char_type* s, size_type pos = 0) const;
    ASSERT_EQ(s.find_first_not_of(s.data()), npos);
    ASSERT_EQ(s.find_first_not_of(s.data(), 0), npos);
    ASSERT_EQ(s.find_first_not_of(chars1.data()), 1u);
    ASSERT_EQ(s.find_first_not_of(chars1.data(), 0), 1u);
    ASSERT_EQ(s.find_first_not_of(chars1.data(), 1), 1u);
    ASSERT_EQ(s.find_first_not_of(chars1.data(), 2), 4u);
    ASSERT_EQ(s.find_first_not_of(chars2.data()), 0u);
    ASSERT_EQ(s.find_first_not_of(chars2.data(), 0), 0u);
    ASSERT_EQ(s.find_first_not_of(chars3.data(), s.size() - 1), s.size() - 1);
    // pos >= size() is safe
    ASSERT_EQ(s.find_first_not_of(chars1.data(), s.size()), npos);
    ASSERT_EQ(s.find_first_not_of(chars1.data(), s.size() + 1024), npos);
    ASSERT_EQ(s.find_first_not_of(chars4.data(), s.size()), npos);

    // find_first_not_of(char_type ch, size_type pos = 0) const;
    ASSERT_EQ(s.find_first_not_of(H), 1u);
    ASSERT_EQ(s.find_first_not_of(H, 0), 1u);
    ASSERT_EQ(s.find_first_not_of(l), 0u);
    ASSERT_EQ(s.find_first_not_of(l, 0), 0u);
    ASSERT_EQ(s.find_first_not_of(l, 2), 4u);
    ASSERT_EQ(sH.find_first_not_of(l), 0u);
    ASSERT_EQ(sH.find_first_not_of(l, 0), 0u);
    ASSERT_EQ(sH.find_first_not_of(l, 1), 1u);
    ASSERT_EQ(sH.find_first_not_of(H), npos);
    ASSERT_EQ(sH.find_first_not_of(H, sH.size() - 1), npos);
    // pos >= size() is safe
    ASSERT_EQ(sH.find_first_not_of(l, sH.size()), npos);
    ASSERT_EQ(sH.find_first_not_of(H, sH.size()), npos);

    // find_first_not_of(const this_type& str, size_type pos = 0) const;
    ASSERT_EQ(s.find_first_not_of(s.data()), npos);
    ASSERT_EQ(s.find_first_not_of(s.data(), 0), npos);
    ASSERT_EQ(s.find_first_not_of(chars1.data()), 1u);
    ASSERT_EQ(s.find_first_not_of(chars1.data(), 0), 1u);
    ASSERT_EQ(s.find_first_not_of(chars1.data(), 1), 1u);
    ASSERT_EQ(s.find_first_not_of(chars1.data(), 2), 4u);
    ASSERT_EQ(s.find_first_not_of(chars2.data()), 0u);
    ASSERT_EQ(s.find_first_not_of(chars2.data(), 0), 0u);
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
    const TestData<CharType> data;
    const auto npos = StringType::npos;
    const auto nul = StringType::nul();

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
    ASSERT_EQ(s.find_last_of(chars1.data(), npos, chars1.size()), 9u);
    ASSERT_EQ(s.find_last_of(chars1.data(), 9, chars1.size()), 9u);
    ASSERT_EQ(s.find_last_of(chars1.data(), 8, chars1.size()), 6u);
    ASSERT_EQ(s.find_last_of(empty.data(), npos, 0), npos);
    ASSERT_EQ(s.find_last_of(empty.data(), 0, 0), npos);
    ASSERT_EQ(s.find_last_of(chars2.data(), npos, chars2.size()), 6u);
    ASSERT_EQ(s.find_last_of(chars3.data(), npos, chars3.size()), npos);
    ASSERT_EQ(s.find_last_of(chars4.data(), npos, chars4.size()), npos);
    ASSERT_EQ(sWithNul.find_last_of(chars4.data(), npos, chars4.size()), sWithNul.size() - 1);
    // "npos" isn't special, it's just a value >= size() - so any value is valid
    ASSERT_EQ(s.find_last_of(chars1.data(), s.size(), chars1.size()), 9u);
    ASSERT_EQ(s.find_last_of(chars1.data(), s.size() + 47, chars1.size()), 9u);
    ASSERT_EQ(s.find_last_of(chars1.data(), s.size() + 2048, chars1.size()), 9u);

    // find_last_of(const char_type* s, size_type pos = npos) const;
    ASSERT_EQ(s.find_last_of(chars1.data()), 9u);
    ASSERT_EQ(s.find_last_of(chars1.data(), npos), 9u);
    ASSERT_EQ(s.find_last_of(chars1.data(), 9), 9u);
    ASSERT_EQ(s.find_last_of(chars1.data(), 8), 6u);
    ASSERT_EQ(s.find_last_of(empty.data()), npos);
    ASSERT_EQ(s.find_last_of(empty.data(), npos), npos);
    ASSERT_EQ(s.find_last_of(chars2.data()), 6u);
    ASSERT_EQ(s.find_last_of(chars2.data(), npos), 6u);
    ASSERT_EQ(s.find_last_of(chars3.data()), npos);
    ASSERT_EQ(s.find_last_of(chars3.data(), npos), npos);
    ASSERT_EQ(s.find_last_of(chars4.data()), npos);
    ASSERT_EQ(s.find_last_of(chars4.data(), npos), npos);
    // finding NUL doesn't work without length
    ASSERT_EQ(sWithNul.find_last_of(chars4.data()), npos);
    ASSERT_EQ(sWithNul.find_last_of(chars4.data(), npos), npos);

    // find_last_of(char_type ch, size_type pos = npos) const;
    ASSERT_EQ(s.find_last_of(s.back()), 11u);
    ASSERT_EQ(s.find_last_of(s.back(), npos), 11u);
    ASSERT_EQ(s.find_last_of(H), 0u);
    ASSERT_EQ(s.find_last_of(H, npos), 0u);
    ASSERT_EQ(s.find_last_of(l), 9u);
    ASSERT_EQ(s.find_last_of(l, npos), 9u);
    ASSERT_EQ(s.find_last_of(l, 9), 9u);
    ASSERT_EQ(s.find_last_of(l, 8), 3u);
    ASSERT_EQ(s.find_last_of(l, 3), 3u);
    ASSERT_EQ(s.find_last_of(l, 2), 2u);
    ASSERT_EQ(s.find_last_of(l, 1), npos);
    ASSERT_EQ(s.find_last_of(l, 0), npos);
    ASSERT_EQ(s.find_last_of(b), npos);
    ASSERT_EQ(s.find_last_of(b, npos), npos);
    ASSERT_EQ(sWithNul.find_last_of(nul, npos), sWithNul.size() - 1);
    ASSERT_EQ(sWithNul.find_last_of(nul, s.size() - 2), npos);

    // find_last_of(const this_type& str, size_type pos = npos) const;
    ASSERT_EQ(s.find_last_of(chars1), 9u);
    ASSERT_EQ(s.find_last_of(chars1, npos), 9u);
    ASSERT_EQ(s.find_last_of(chars1, 9), 9u);
    ASSERT_EQ(s.find_last_of(chars1, 8), 6u);
    ASSERT_EQ(s.find_last_of(empty), npos);
    ASSERT_EQ(s.find_last_of(empty, npos), npos);
    ASSERT_EQ(s.find_last_of(empty, 0), npos);
    ASSERT_EQ(s.find_last_of(chars2), 6u);
    ASSERT_EQ(s.find_last_of(chars2, npos), 6u);
    ASSERT_EQ(s.find_last_of(chars3), npos);
    ASSERT_EQ(s.find_last_of(chars3, npos), npos);
    ASSERT_EQ(s.find_last_of(chars4), npos);
    ASSERT_EQ(s.find_last_of(chars4, npos), npos);
    ASSERT_EQ(sWithNul.find_last_of(chars4), sWithNul.size() - 1);
    ASSERT_EQ(sWithNul.find_last_of(chars4, npos), sWithNul.size() - 1);
    // "npos" isn't special, it's just a value >= size() - so any value is valid
    ASSERT_EQ(s.find_last_of(chars1, s.size()), 9u);
    ASSERT_EQ(s.find_last_of(chars1, s.size() + 47), 9u);
    ASSERT_EQ(s.find_last_of(chars1, s.size() + 2048), 9u);
}

/* find_last_not_of functions */
TYPED_TEST(StringBaseTest, FindLastNotOfFunctions)
{
    using StringType = TypeParam; // gtest specific
    using StorageType = typename StringType::storage_type;
    using CharType = typename StorageType::char_type;
    const TestData<CharType> data;
    const auto npos = StringType::npos;
    const auto nul = StringType::nul();

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
    ASSERT_EQ(s.find_last_not_of(chars1.data(), npos, chars1.size()), 11u);
    ASSERT_EQ(s.find_last_not_of(chars1.data(), 6, chars1.size()), 5u);
    ASSERT_EQ(s.find_last_not_of(chars2.data(), 6, chars2.size()), 5u);
    ASSERT_EQ(s.find_last_not_of(chars3.data(), npos, chars3.size()), 11u);
    ASSERT_EQ(s.find_last_not_of(chars3.data(), 11, chars3.size()), 11u);
    ASSERT_EQ(s.find_last_not_of(chars3.data(), 10, chars3.size()), 10u);
    ASSERT_EQ(s.find_last_not_of(chars3.data(), 9, chars3.size()), 9u);
    ASSERT_EQ(sWithNul.find_last_not_of(chars4.data(), npos, chars4.size()), sWithNul.size() - 2);
    // "npos" isn't special, it's just a value >= size() - so any value is valid
    ASSERT_EQ(s.find_last_not_of(chars1.data(), s.size(), chars1.size()), 11u);
    ASSERT_EQ(s.find_last_not_of(chars1.data(), s.size() + 47, chars1.size()), 11u);
    ASSERT_EQ(s.find_last_not_of(chars1.data(), s.size() + 2048, chars1.size()), 11u);

    // find_last_not_of(const char_type* s, size_type pos = npos) const;
    ASSERT_EQ(s.find_last_not_of(empty.data()), s.size() - 1);
    ASSERT_EQ(s.find_last_not_of(empty.data(), npos), s.size() - 1);
    ASSERT_EQ(s.find_last_not_of(s.data()), npos);
    ASSERT_EQ(s.find_last_not_of(s.data(), npos), npos);
    ASSERT_EQ(s.find_last_not_of(s.data(), 0), npos);
    ASSERT_EQ(s.find_last_not_of(chars1.data()), 11u);
    ASSERT_EQ(s.find_last_not_of(chars1.data(), npos), 11u);
    ASSERT_EQ(s.find_last_not_of(chars1.data(), 6, chars1.size()), 5u);
    ASSERT_EQ(s.find_last_not_of(chars2.data(), 6, chars2.size()), 5u);
    ASSERT_EQ(s.find_last_not_of(chars3.data()), 11u);
    ASSERT_EQ(s.find_last_not_of(chars3.data(), npos), 11u);
    ASSERT_EQ(s.find_last_not_of(chars3.data(), 11), 11u);
    ASSERT_EQ(s.find_last_not_of(chars3.data(), 10), 10u);
    ASSERT_EQ(s.find_last_not_of(chars3.data(), 9), 9u);
    // finding NUL doesn't work without length
    ASSERT_EQ(sWithNul.find_last_not_of(chars4.data(), npos), sWithNul.size() - 1);

    // find_last_not_of(char_type ch, size_type pos = npos) const;
    ASSERT_EQ(s.find_last_not_of(s.back()), 10u);
    ASSERT_EQ(s.find_last_not_of(s.back(), npos), 10u);
    ASSERT_EQ(s.find_last_not_of(H), 11u);
    ASSERT_EQ(s.find_last_not_of(H, npos), 11u);
    ASSERT_EQ(s.find_last_not_of(H, 0), npos);
    ASSERT_EQ(s.find_last_not_of(W, 6), 5u);
    ASSERT_EQ(s.find_last_not_of(l), 11u);
    ASSERT_EQ(s.find_last_not_of(l, npos), 11u);
    ASSERT_EQ(s.find_last_not_of(l, 9), 8u);
    ASSERT_EQ(sWithNul.find_last_not_of(b), sWithNul.size() - 1);
    ASSERT_EQ(sWithNul.find_last_not_of(nul), sWithNul.size() - 2);

    // find_last_not_of(const this_type& str, size_type pos = npos) const;
    ASSERT_EQ(s.find_last_not_of(empty), s.size() - 1);
    ASSERT_EQ(s.find_last_not_of(empty, npos), s.size() - 1);
    ASSERT_EQ(s.find_last_not_of(s), npos);
    ASSERT_EQ(s.find_last_not_of(s, npos), npos);
    ASSERT_EQ(s.find_last_not_of(s, 0), npos);
    ASSERT_EQ(s.find_last_not_of(chars1), 11u);
    ASSERT_EQ(s.find_last_not_of(chars1, npos), 11u);
    ASSERT_EQ(s.find_last_not_of(chars1, 6), 5u);
    ASSERT_EQ(s.find_last_not_of(chars2, 6), 5u);
    ASSERT_EQ(s.find_last_not_of(chars3), 11u);
    ASSERT_EQ(s.find_last_not_of(chars3, npos), 11u);
    ASSERT_EQ(s.find_last_not_of(chars3, 11), 11u);
    ASSERT_EQ(s.find_last_not_of(chars3, 10), 10u);
    ASSERT_EQ(s.find_last_not_of(chars3, 9), 9u);
    ASSERT_EQ(sWithNul.find_last_not_of(chars4), sWithNul.size() - 2);
    ASSERT_EQ(sWithNul.find_last_not_of(chars4, npos), sWithNul.size() - 2);
    // "npos" isn't special, it's just a value >= size() - so any value is valid
    ASSERT_EQ(s.find_last_not_of(chars1, s.size()), 11u);
    ASSERT_EQ(s.find_last_not_of(chars1, s.size() + 47), 11u);
    ASSERT_EQ(s.find_last_not_of(chars1, s.size() + 2048), 11u);
}
