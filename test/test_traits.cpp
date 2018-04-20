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

// Note: wrapper macros to ensure that GTest gets actual variables (required in order to print them)

#define ASSERT_HAS_SIZE(S, T)                                                                      \
    {                                                                                              \
        const bool hasSize = spsl::has_size<S, T>::value;                                          \
        ASSERT_TRUE(hasSize);                                                                      \
    }

#define ASSERT_HAS_NO_SIZE(S, T)                                                                   \
    {                                                                                              \
        const bool hasSize = spsl::has_size<S, T>::value;                                          \
        ASSERT_FALSE(hasSize);                                                                     \
    }

#define ASSERT_HAS_DATA(S, T)                                                                      \
    {                                                                                              \
        const bool hasData = spsl::has_data<S, T>::value;                                          \
        ASSERT_TRUE(hasData);                                                                      \
    }

#define ASSERT_HAS_NO_DATA(S, T)                                                                   \
    {                                                                                              \
        const bool hasData = spsl::has_data<S, T>::value;                                          \
        ASSERT_FALSE(hasData);                                                                     \
    }

#define ASSERT_IS_COMPAT(C, S, T)                                                                  \
    {                                                                                              \
        const bool isCompat = spsl::is_compatible_string<C, S, T>::value;                          \
        ASSERT_TRUE(isCompat);                                                                     \
    }

#define ASSERT_IS_NOT_COMPAT(C, S, T)                                                              \
    {                                                                                              \
        const bool isCompat = spsl::is_compatible_string<C, S, T>::value;                          \
        ASSERT_FALSE(isCompat);                                                                    \
    }

// test the has_size traits template
TEST(TypeTraitsTest, HasSize)
{
    ASSERT_HAS_SIZE(size_t, std::string);
    ASSERT_HAS_SIZE(size_t, std::wstring);

    ASSERT_HAS_SIZE(size_t, std::vector<char>);
    ASSERT_HAS_SIZE(size_t, std::vector<wchar_t>);

    ASSERT_HAS_SIZE(size_t, spsl::ArrayString<32>);
    ASSERT_HAS_SIZE(size_t, spsl::ArrayStringW<32>);
    ASSERT_HAS_SIZE(size_t, spsl::PasswordString);
    ASSERT_HAS_SIZE(size_t, spsl::PasswordStringW);

#ifdef TEST_STRING_VIEW
    ASSERT_HAS_SIZE(size_t, std::experimental::string_view);
    ASSERT_HAS_SIZE(size_t, std::experimental::wstring_view);
#endif

    ASSERT_HAS_NO_SIZE(size_t, const char*);
    ASSERT_HAS_NO_SIZE(size_t, std::exception);
}

// test the has_data traits template
TEST(TypeTraitsTest, HasData)
{
    ASSERT_HAS_DATA(char, std::string);
    ASSERT_HAS_DATA(wchar_t, std::wstring);
    ASSERT_HAS_NO_DATA(wchar_t, std::string);
    ASSERT_HAS_NO_DATA(char, std::wstring);

    ASSERT_HAS_DATA(char, std::vector<char>);
    ASSERT_HAS_DATA(wchar_t, std::vector<wchar_t>);
    ASSERT_HAS_NO_DATA(wchar_t, std::vector<char>);
    ASSERT_HAS_NO_DATA(char, std::vector<wchar_t>);

    ASSERT_HAS_DATA(char, spsl::ArrayString<32>);
    ASSERT_HAS_DATA(wchar_t, spsl::ArrayStringW<32>);
    ASSERT_HAS_DATA(char, spsl::PasswordString);
    ASSERT_HAS_DATA(wchar_t, spsl::PasswordStringW);
    ASSERT_HAS_NO_DATA(wchar_t, spsl::ArrayString<32>);
    ASSERT_HAS_NO_DATA(char, spsl::ArrayStringW<32>);
    ASSERT_HAS_NO_DATA(wchar_t, spsl::PasswordString);
    ASSERT_HAS_NO_DATA(char, spsl::PasswordStringW);

#ifdef TEST_STRING_VIEW
    ASSERT_HAS_DATA(char, std::experimental::string_view);
    ASSERT_HAS_DATA(wchar_t, std::experimental::wstring_view);
#endif

    ASSERT_HAS_NO_DATA(char, const char*);
    ASSERT_HAS_NO_DATA(char, std::exception);
}

// test the is_compatible_string traits template
TEST(TypeTraitsTest, IsCompatible)
{
    ASSERT_IS_COMPAT(char, size_t, std::string);
    ASSERT_IS_COMPAT(wchar_t, size_t, std::wstring);
    ASSERT_IS_NOT_COMPAT(wchar_t, size_t, std::string);
    ASSERT_IS_NOT_COMPAT(char, size_t, std::wstring);

    ASSERT_IS_COMPAT(char, size_t, std::vector<char>);
    ASSERT_IS_COMPAT(wchar_t, size_t, std::vector<wchar_t>);
    ASSERT_IS_NOT_COMPAT(wchar_t, size_t, std::vector<char>);
    ASSERT_IS_NOT_COMPAT(char, size_t, std::vector<wchar_t>);

    ASSERT_IS_COMPAT(char, size_t, spsl::ArrayString<32>);
    ASSERT_IS_COMPAT(wchar_t, size_t, spsl::ArrayStringW<32>);
    ASSERT_IS_COMPAT(char, size_t, spsl::PasswordString);
    ASSERT_IS_COMPAT(wchar_t, size_t, spsl::PasswordStringW);
    ASSERT_IS_NOT_COMPAT(wchar_t, size_t, spsl::ArrayString<32>);
    ASSERT_IS_NOT_COMPAT(char, size_t, spsl::ArrayStringW<32>);
    ASSERT_IS_NOT_COMPAT(wchar_t, size_t, spsl::PasswordString);
    ASSERT_IS_NOT_COMPAT(char, size_t, spsl::PasswordStringW);

#ifdef TEST_STRING_VIEW
    ASSERT_IS_COMPAT(char, size_t, std::experimental::string_view);
    ASSERT_IS_COMPAT(wchar_t, size_t, std::experimental::wstring_view);
#endif

    ASSERT_IS_NOT_COMPAT(char, size_t, const char*);
    ASSERT_IS_NOT_COMPAT(char, size_t, std::exception);
    ASSERT_IS_NOT_COMPAT(char, size_t, std::vector<int>);
}
