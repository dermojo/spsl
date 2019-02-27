/**
 * @file	Special Purpose Strings Library: test_stringcore_construct.cpp
 * @author	Daniel Evers
 * @brief	StringCore unit tests: constructor tests
 * @license MIT
 */

#include <gtest/gtest.h>

#include "spsl.hpp"
#include "testdata.hpp"
#include "tests.hpp"

/* check constructor availability */
TYPED_TEST(StringCoreTest, Constructors)
{
    using StringType = TypeParam; // gtest specific
    using StorageType = typename StringType::storage_type;
    using CharType = typename StorageType::char_type;
    using Traits = typename StringType::traits_type;
    const TestData<CharType> data;

    // default constructor
    {
        const StringType s;
        ASSERT_EQ(s.length(), 0u);
        ASSERT_EQ(s.size(), 0u);
        ASSERT_TRUE(s.empty());
        // same pointer
        ASSERT_EQ(s.c_str(), s.data());
        ASSERT_TRUE(s == data.empty);
        ASSERT_TRUE(s != data.hello_world);
    }

    // construct from string (without length)
    {
        const StringType s(data.hello_world);
        ASSERT_EQ(s.length(), data.hello_world_len);
        ASSERT_EQ(s.size(), data.hello_world_len);
        ASSERT_FALSE(s.empty());
        ASSERT_EQ(s.c_str(), s.data());
        ASSERT_TRUE(Traits::compare(s.c_str(), data.hello_world, data.hello_world_len) == 0);
        ASSERT_TRUE(s == data.hello_world);
        ASSERT_TRUE(s != data.empty);
    }

    // construct from string (with length)
    {
        const size_t length = data.hello_world_len - 2u;
        const StringType s(data.hello_world, length);
        ASSERT_EQ(s.length(), length);
        ASSERT_EQ(s.size(), length);
        ASSERT_FALSE(s.empty());
        ASSERT_EQ(s.c_str(), s.data());
        ASSERT_TRUE(Traits::compare(s.c_str(), data.hello_world, length) == 0);
        ASSERT_FALSE(s == data.hello_world);
        ASSERT_TRUE(s != data.empty);
    }

    // construct from a repeating character
    {
        const size_t length = 64;
        const CharType ch = data.blablabla[0];
        const StringType s(length, ch);
        ASSERT_EQ(s.length(), length);
        ASSERT_EQ(s.size(), length);
        ASSERT_FALSE(s.empty());
        ASSERT_EQ(s.c_str(), s.data());
        for (size_t i = 0; i < s.length(); ++i)
            ASSERT_TRUE(s[i] == ch);
        ASSERT_TRUE(s != data.blablabla);
        ASSERT_TRUE(s != data.hello_world);
        ASSERT_TRUE(s != data.empty);
    }

    // construct from initializer list
    {
        const CharType ch = data.blablabla[0];
        const StringType s{ ch, ch, ch, ch, ch };
        ASSERT_EQ(s.length(), 5u);
        ASSERT_EQ(s.size(), 5u);
        ASSERT_FALSE(s.empty());
        ASSERT_EQ(s.c_str(), s.data());
        for (size_t i = 0; i < s.length(); ++i)
            ASSERT_TRUE(s[i] == ch);
    }

    // copy constructor
    {
        const StringType s1(data.hello_world);
        const StringType s2(s1);
        ASSERT_EQ(s1.length(), data.hello_world_len);
        ASSERT_EQ(s1.size(), data.hello_world_len);
        ASSERT_EQ(s2.length(), data.hello_world_len);
        ASSERT_EQ(s2.size(), data.hello_world_len);
        ASSERT_EQ(s1, s2);
        // compare including NUL
        for (size_t i = 0; i <= s1.length(); ++i)
            ASSERT_TRUE(s1[i] == s2[i]);
    }

    // move constructor
    {
        StringType s1(data.hello_world);
        const StringType s2(std::move(s1));
        // assume that the moved-from string is either unchanged or empty
        ASSERT_TRUE(s1.empty() || s1.length() == data.hello_world_len);
        ASSERT_TRUE(s1.empty() || s1 == data.hello_world);
        ASSERT_EQ(s2.length(), data.hello_world_len);
        ASSERT_EQ(s2.size(), data.hello_world_len);
        ASSERT_TRUE(Traits::compare(s2.c_str(), data.hello_world, data.hello_world_len) == 0);
        ASSERT_TRUE(s2 == data.hello_world);
    }

    // construct from a storage instance
    {
        StorageType st;
        st.assign(data.hello_world, data.hello_world_len);
        StringType s(st);
        ASSERT_EQ(s.length(), data.hello_world_len);
        ASSERT_EQ(s.size(), data.hello_world_len);
        ASSERT_TRUE(Traits::compare(s.c_str(), data.hello_world, data.hello_world_len) == 0);
        ASSERT_TRUE(s == data.hello_world);
    }

    // construct from another string-like container
    {
        // 1: from a std::basic_string
        {
            std::basic_string<CharType> ref;
            ref = data.hello_world;
            StringType s(ref);
            ASSERT_EQ(s.length(), data.hello_world_len);
            ASSERT_EQ(s.size(), data.hello_world_len);
            ASSERT_TRUE(Traits::compare(s.c_str(), data.hello_world, data.hello_world_len) == 0);
            ASSERT_TRUE(s == data.hello_world);
        }

        // 2: from a std::vector
        {
            std::vector<CharType> ref;
            ref.resize(data.hello_world_len);
            memcpy(ref.data(), data.hello_world, data.hello_world_len * sizeof(CharType));
            StringType s(ref);
            ASSERT_EQ(s.length(), data.hello_world_len);
            ASSERT_EQ(s.size(), data.hello_world_len);
            ASSERT_TRUE(Traits::compare(s.c_str(), data.hello_world, data.hello_world_len) == 0);
            ASSERT_TRUE(s == data.hello_world);
        }

        // 3: from another version of StringCore
        {
            const spsl::StringCore<
              spsl::StorageArray<CharType, 123, spsl::policy::overflow::Truncate>>
              other(data.hello_world);
            StringType s(other);
            ASSERT_EQ(s.length(), data.hello_world_len);
            ASSERT_EQ(s.size(), data.hello_world_len);
            ASSERT_TRUE(Traits::compare(s.c_str(), data.hello_world, data.hello_world_len) == 0);
            ASSERT_TRUE(s == data.hello_world);
        }
    }
}

/* check constructor availability */
TYPED_TEST(StringCoreTest, ConstructorIterator)
{
    using StringType = TypeParam; // gtest specific
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
            ASSERT_EQ(s.length(), data.hello_world_len);
            ASSERT_EQ(s.size(), data.hello_world_len);
            ASSERT_TRUE(Traits::compare(s.c_str(), data.hello_world, data.hello_world_len) == 0)
              << hexdump(s.c_str(), s.size()) << "\n"
              << hexdump(data.hello_world, data.hello_world_len);
            ASSERT_TRUE(s == data.hello_world);
        }

        // 2: from a std::vector
        {
            std::vector<CharType> ref;
            ref.resize(data.hello_world_len);
            memcpy(ref.data(), data.hello_world, data.hello_world_len * sizeof(CharType));
            StringType s(ref.begin(), ref.end());
            ASSERT_EQ(s.length(), data.hello_world_len);
            ASSERT_EQ(s.size(), data.hello_world_len);
            ASSERT_TRUE(Traits::compare(s.c_str(), data.hello_world, data.hello_world_len) == 0);
            ASSERT_TRUE(s == data.hello_world);
        }

        // 3: from another version of StringCore
        {
            const spsl::StringCore<
              spsl::StorageArray<CharType, 123, spsl::policy::overflow::Truncate>>
              other(data.hello_world);
            StringType s(other.begin(), other.end());
            ASSERT_EQ(s.length(), data.hello_world_len);
            ASSERT_EQ(s.size(), data.hello_world_len);
            ASSERT_TRUE(Traits::compare(s.c_str(), data.hello_world, data.hello_world_len) == 0)
              << hexdump(s.data(), s.size() * sizeof(CharType));
            ASSERT_TRUE(s == data.hello_world);
        }
    }
}
