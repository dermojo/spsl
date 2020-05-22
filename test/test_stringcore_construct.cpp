/**
 * @file	Special Purpose Strings Library: test_stringcore_construct.cpp
 * @author	Daniel Evers
 * @brief	StringCore unit tests: constructor tests
 * @license MIT
 */

#include "doctest.h"

#include "spsl.hpp"
#include "testdata.hpp"
#include "tests.hpp"

/* check constructor availability */
TEST_CASE_TEMPLATE_DEFINE("StringCore constructors", StringType, StringCore_constructors)
{
    using StorageType = typename StringType::storage_type;
    using CharType = typename StorageType::char_type;
    using Traits = typename StringType::traits_type;
    const TestData<CharType> data;

    // default constructor
    {
        const StringType s;
        REQUIRE(s.length() == 0u);
        REQUIRE(s.size() == 0u);
        REQUIRE(s.empty());
        // same pointer
        REQUIRE(s.c_str() == s.data());
        REQUIRE(s == data.empty);
        REQUIRE(s != data.hello_world);
    }

    // construct from string (without length)
    {
        const StringType s(data.hello_world);
        REQUIRE(s.length() == data.hello_world_len);
        REQUIRE(s.size() == data.hello_world_len);
        REQUIRE_FALSE(s.empty());
        REQUIRE(s.c_str() == s.data());
        REQUIRE(Traits::compare(s.c_str(), data.hello_world, data.hello_world_len) == 0);
        REQUIRE(s == data.hello_world);
        REQUIRE(s != data.empty);
    }

    // construct from string (with length)
    {
        const size_t length = data.hello_world_len - 2u;
        const StringType s(data.hello_world, length);
        REQUIRE(s.length() == length);
        REQUIRE(s.size() == length);
        REQUIRE_FALSE(s.empty());
        REQUIRE(s.c_str() == s.data());
        REQUIRE(Traits::compare(s.c_str(), data.hello_world, length) == 0);
        REQUIRE_FALSE(s == data.hello_world);
        REQUIRE(s != data.empty);
    }

    // construct from a repeating character
    {
        const size_t length = 64;
        const CharType ch = data.blablabla[0];
        const StringType s(length, ch);
        REQUIRE(s.length() == length);
        REQUIRE(s.size() == length);
        REQUIRE_FALSE(s.empty());
        REQUIRE(s.c_str() == s.data());
        for (size_t i = 0; i < s.length(); ++i)
            REQUIRE(s[i] == ch);
        REQUIRE(s != data.blablabla);
        REQUIRE(s != data.hello_world);
        REQUIRE(s != data.empty);
    }

    // construct from initializer list
    {
        const CharType ch = data.blablabla[0];
        const StringType s{ ch, ch, ch, ch, ch };
        REQUIRE(s.length() == 5u);
        REQUIRE(s.size() == 5u);
        REQUIRE_FALSE(s.empty());
        REQUIRE(s.c_str() == s.data());
        for (size_t i = 0; i < s.length(); ++i)
            REQUIRE(s[i] == ch);
    }

    // copy constructor
    {
        const StringType s1(data.hello_world);
        const StringType s2(s1);
        REQUIRE(s1.length() == data.hello_world_len);
        REQUIRE(s1.size() == data.hello_world_len);
        REQUIRE(s2.length() == data.hello_world_len);
        REQUIRE(s2.size() == data.hello_world_len);
        REQUIRE(s1 == s2);
        // compare including NUL
        for (size_t i = 0; i <= s1.length(); ++i)
            REQUIRE(s1[i] == s2[i]);
    }

    // move constructor
    {
        StringType s1(data.hello_world);
        const StringType s2(std::move(s1));
        // assume that the moved-from string is either unchanged or empty
        REQUIRE((s1.empty() || s1.length() == data.hello_world_len));
        REQUIRE((s1.empty() || s1 == data.hello_world));
        REQUIRE(s2.length() == data.hello_world_len);
        REQUIRE(s2.size() == data.hello_world_len);
        REQUIRE(Traits::compare(s2.c_str(), data.hello_world, data.hello_world_len) == 0);
        REQUIRE(s2 == data.hello_world);
    }

    // construct from a storage instance
    {
        StorageType st;
        st.assign(data.hello_world, data.hello_world_len);
        StringType s(st);
        REQUIRE(s.length() == data.hello_world_len);
        REQUIRE(s.size() == data.hello_world_len);
        REQUIRE(Traits::compare(s.c_str(), data.hello_world, data.hello_world_len) == 0);
        REQUIRE(s == data.hello_world);
    }

    // construct from another string-like container
    {
        // 1: from a std::basic_string
        {
            std::basic_string<CharType> ref;
            ref = data.hello_world;
            StringType s(ref);
            REQUIRE(s.length() == data.hello_world_len);
            REQUIRE(s.size() == data.hello_world_len);
            REQUIRE(Traits::compare(s.c_str(), data.hello_world, data.hello_world_len) == 0);
            REQUIRE(s == data.hello_world);
        }

        // 2: from a std::vector
        {
            std::vector<CharType> ref;
            ref.resize(data.hello_world_len);
            memcpy(ref.data(), data.hello_world, data.hello_world_len * sizeof(CharType));
            StringType s(ref);
            REQUIRE(s.length() == data.hello_world_len);
            REQUIRE(s.size() == data.hello_world_len);
            REQUIRE(Traits::compare(s.c_str(), data.hello_world, data.hello_world_len) == 0);
            REQUIRE(s == data.hello_world);
        }

        // 3: from another version of StringCore
        {
            const spsl::StringCore<
              spsl::StorageArray<CharType, 123, spsl::policy::overflow::Truncate>>
              other(data.hello_world);
            StringType s(other);
            REQUIRE(s.length() == data.hello_world_len);
            REQUIRE(s.size() == data.hello_world_len);
            REQUIRE(Traits::compare(s.c_str(), data.hello_world, data.hello_world_len) == 0);
            REQUIRE(s == data.hello_world);
        }
    }
}
TEST_CASE_TEMPLATE_APPLY(StringCore_constructors, StringCoreTestTypes);

/* check constructor availability */
TEST_CASE_TEMPLATE_DEFINE("StringCore constructor iterator", StringType, StringCore_construct_iter)
{
    using StorageType = typename StringType::storage_type;
    using CharType = typename StorageType::char_type;
    using Traits = typename StringType::traits_type;
    const TestData<CharType> data;

    // construct from input iterators
    {
        // 1: from a std::basic_string
        {
            std::basic_string<CharType> ref;
            ref = data.hello_world;
            StringType s(ref.begin(), ref.end());
            REQUIRE(s.length() == data.hello_world_len);
            REQUIRE(s.size() == data.hello_world_len);
            REQUIRE(Traits::compare(s.c_str(), data.hello_world, data.hello_world_len) == 0);
            REQUIRE(s == data.hello_world);
        }

        // 2: from a std::vector
        {
            std::vector<CharType> ref;
            ref.resize(data.hello_world_len);
            memcpy(ref.data(), data.hello_world, data.hello_world_len * sizeof(CharType));
            StringType s(ref.begin(), ref.end());
            REQUIRE(s.length() == data.hello_world_len);
            REQUIRE(s.size() == data.hello_world_len);
            REQUIRE(Traits::compare(s.c_str(), data.hello_world, data.hello_world_len) == 0);
            REQUIRE(s == data.hello_world);
        }

        // 3: from another version of StringCore
        {
            const spsl::StringCore<
              spsl::StorageArray<CharType, 123, spsl::policy::overflow::Truncate>>
              other(data.hello_world);
            StringType s(other.begin(), other.end());
            REQUIRE(s.length() == data.hello_world_len);
            REQUIRE(s.size() == data.hello_world_len);
            REQUIRE(Traits::compare(s.c_str(), data.hello_world, data.hello_world_len) == 0);
            REQUIRE(s == data.hello_world);
        }
    }
}
TEST_CASE_TEMPLATE_APPLY(StringCore_construct_iter, StringCoreTestTypes);
