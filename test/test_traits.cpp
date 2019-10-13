/**
 * @file    Special Purpose Strings Library: test_traits.cpp
 * @author  Daniel Evers
 * @brief   Type traits unit tests
 * @license MIT
 */

#include <vector>

#include "catch.hpp"
#include "spsl.hpp"
#include "testdata.hpp"


// Note: wrapper macros to ensure that GTest gets actual variables (required in order to print them)

#define ASSERT_HAS_SIZE(S, T)                                                                      \
    {                                                                                              \
        const bool hasSize = spsl::has_size<S, T>::value;                                          \
        REQUIRE(hasSize == true);                                                                  \
    }

#define ASSERT_HAS_NO_SIZE(S, T)                                                                   \
    {                                                                                              \
        const bool hasSize = spsl::has_size<S, T>::value;                                          \
        REQUIRE(hasSize == false);                                                                 \
    }

#define ASSERT_HAS_DATA(S, T)                                                                      \
    {                                                                                              \
        const bool hasData = spsl::has_data<S, T>::value;                                          \
        REQUIRE(hasData == true);                                                                  \
    }

#define ASSERT_HAS_NO_DATA(S, T)                                                                   \
    {                                                                                              \
        const bool hasData = spsl::has_data<S, T>::value;                                          \
        REQUIRE(hasData == false);                                                                 \
    }

#define ASSERT_IS_COMPAT(C, S, T)                                                                  \
    {                                                                                              \
        const bool isCompat = spsl::is_compatible_string<C, S, T>::value;                          \
        REQUIRE(isCompat == true);                                                                 \
    }

#define ASSERT_IS_NOT_COMPAT(C, S, T)                                                              \
    {                                                                                              \
        const bool isCompat = spsl::is_compatible_string<C, S, T>::value;                          \
        REQUIRE(isCompat == false);                                                                \
    }

// test the has_size traits template
TEST_CASE("has_size", "[traits]")
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
    ASSERT_HAS_SIZE(size_t, std::string_view);
    ASSERT_HAS_SIZE(size_t, std::wstring_view);
#endif

    ASSERT_HAS_NO_SIZE(size_t, const char*);
    ASSERT_HAS_NO_SIZE(size_t, std::exception);
}

// test the has_data traits template
TEST_CASE("has_data", "[traits]")
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
    ASSERT_HAS_DATA(char, std::string_view);
    ASSERT_HAS_DATA(wchar_t, std::wstring_view);
#endif

    ASSERT_HAS_NO_DATA(char, const char*);
    ASSERT_HAS_NO_DATA(char, std::exception);
}

// test the is_compatible_string traits template
TEST_CASE("is_compatible", "[traits]")
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
    ASSERT_IS_COMPAT(char, size_t, std::string_view);
    ASSERT_IS_COMPAT(wchar_t, size_t, std::wstring_view);
#endif

    ASSERT_IS_NOT_COMPAT(char, size_t, const char*);
    ASSERT_IS_NOT_COMPAT(char, size_t, std::exception);
    ASSERT_IS_NOT_COMPAT(char, size_t, std::vector<int>);
}
