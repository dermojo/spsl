/**
 * @file    Special Purpose Strings Library: test_traits.cpp
 * @author  Daniel Evers
 * @brief   Type traits unit tests
 * @license MIT
 */

#include <gtest/gtest.h>
#include <vector>

#include "spsl.hpp"
#include "testdata.hpp"

template <typename CharType>
class TypeTraitsTest : public ::testing::Test
{
};

// test the has_size traits template
TEST(TypeTraitsTest, HasSize)
{
    ASSERT_TRUE((spsl::has_size<size_t, std::string>::value));
    ASSERT_TRUE((spsl::has_size<size_t, std::wstring>::value));

    ASSERT_TRUE((spsl::has_size<size_t, std::vector<char>>::value));
    ASSERT_TRUE((spsl::has_size<size_t, std::vector<wchar_t>>::value));

    ASSERT_TRUE((spsl::has_size<size_t, spsl::ArrayString<32>>::value));
    ASSERT_TRUE((spsl::has_size<size_t, spsl::ArrayStringW<32>>::value));
    ASSERT_TRUE((spsl::has_size<size_t, spsl::PasswordString>::value));
    ASSERT_TRUE((spsl::has_size<size_t, spsl::PasswordStringW>::value));

#ifdef TEST_STRING_VIEW
    ASSERT_TRUE((spsl::has_size<size_t, std::experimental::string_view>::value));
    ASSERT_TRUE((spsl::has_size<size_t, std::experimental::wstring_view>::value));
#endif

    ASSERT_FALSE((spsl::has_size<size_t, const char*>::value));
    ASSERT_FALSE((spsl::has_size<size_t, std::exception>::value));
}

// test the has_data traits template
TEST(TypeTraitsTest, HasData)
{
    ASSERT_TRUE((spsl::has_data<char, std::string>::value));
    ASSERT_TRUE((spsl::has_data<wchar_t, std::wstring>::value));
    ASSERT_FALSE((spsl::has_data<wchar_t, std::string>::value));
    ASSERT_FALSE((spsl::has_data<char, std::wstring>::value));

    ASSERT_TRUE((spsl::has_data<char, std::vector<char>>::value));
    ASSERT_TRUE((spsl::has_data<wchar_t, std::vector<wchar_t>>::value));
    ASSERT_FALSE((spsl::has_data<wchar_t, std::vector<char>>::value));
    ASSERT_FALSE((spsl::has_data<char, std::vector<wchar_t>>::value));

    ASSERT_TRUE((spsl::has_data<char, spsl::ArrayString<32>>::value));
    ASSERT_TRUE((spsl::has_data<wchar_t, spsl::ArrayStringW<32>>::value));
    ASSERT_TRUE((spsl::has_data<char, spsl::PasswordString>::value));
    ASSERT_TRUE((spsl::has_data<wchar_t, spsl::PasswordStringW>::value));
    ASSERT_FALSE((spsl::has_data<wchar_t, spsl::ArrayString<32>>::value));
    ASSERT_FALSE((spsl::has_data<char, spsl::ArrayStringW<32>>::value));
    ASSERT_FALSE((spsl::has_data<wchar_t, spsl::PasswordString>::value));
    ASSERT_FALSE((spsl::has_data<char, spsl::PasswordStringW>::value));

#ifdef TEST_STRING_VIEW
    ASSERT_TRUE((spsl::has_data<char, std::experimental::string_view>::value));
    ASSERT_TRUE((spsl::has_data<wchar_t, std::experimental::wstring_view>::value));
#endif

    ASSERT_FALSE((spsl::has_data<char, const char*>::value));
    ASSERT_FALSE((spsl::has_data<char, std::exception>::value));
}

// test the is_compatible_string traits template
TEST(TypeTraitsTest, IsCompatible)
{
    ASSERT_TRUE((spsl::is_compatible_string<char, size_t, std::string>::value));
    ASSERT_TRUE((spsl::is_compatible_string<wchar_t, size_t, std::wstring>::value));
    ASSERT_FALSE((spsl::is_compatible_string<wchar_t, size_t, std::string>::value));
    ASSERT_FALSE((spsl::is_compatible_string<char, size_t, std::wstring>::value));

    ASSERT_TRUE((spsl::is_compatible_string<char, size_t, std::vector<char>>::value));
    ASSERT_TRUE((spsl::is_compatible_string<wchar_t, size_t, std::vector<wchar_t>>::value));
    ASSERT_FALSE((spsl::is_compatible_string<wchar_t, size_t, std::vector<char>>::value));
    ASSERT_FALSE((spsl::is_compatible_string<char, size_t, std::vector<wchar_t>>::value));

    ASSERT_TRUE((spsl::is_compatible_string<char, size_t, spsl::ArrayString<32>>::value));
    ASSERT_TRUE((spsl::is_compatible_string<wchar_t, size_t, spsl::ArrayStringW<32>>::value));
    ASSERT_TRUE((spsl::is_compatible_string<char, size_t, spsl::PasswordString>::value));
    ASSERT_TRUE((spsl::is_compatible_string<wchar_t, size_t, spsl::PasswordStringW>::value));
    ASSERT_FALSE((spsl::is_compatible_string<wchar_t, size_t, spsl::ArrayString<32>>::value));
    ASSERT_FALSE((spsl::is_compatible_string<char, size_t, spsl::ArrayStringW<32>>::value));
    ASSERT_FALSE((spsl::is_compatible_string<wchar_t, size_t, spsl::PasswordString>::value));
    ASSERT_FALSE((spsl::is_compatible_string<char, size_t, spsl::PasswordStringW>::value));

#ifdef TEST_STRING_VIEW
    ASSERT_TRUE((spsl::is_compatible_string<char, size_t, std::experimental::string_view>::value));
    ASSERT_TRUE(
      (spsl::is_compatible_string<wchar_t, size_t, std::experimental::wstring_view>::value));
#endif

    ASSERT_FALSE((spsl::is_compatible_string<char, size_t, const char*>::value));
    ASSERT_FALSE((spsl::is_compatible_string<char, size_t, std::exception>::value));
    ASSERT_FALSE((spsl::is_compatible_string<char, size_t, std::vector<int>>::value));
}
