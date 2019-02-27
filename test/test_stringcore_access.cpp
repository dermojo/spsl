/**
 * @file	Special Purpose Strings Library: test_stringcore_access.cpp
 * @author	Daniel Evers
 * @brief	StringCore unit tests: various accessor functions
 * @license MIT
 */

#include <gtest/gtest.h>

#include "spsl.hpp"
#include "testdata.hpp"
#include "tests.hpp"


/* access functions */
TYPED_TEST(StringCoreTest, AccessFunctions)
{
    using StringType = TypeParam; // gtest specific
    using StorageType = typename StringType::storage_type;
    using CharType = typename StorageType::char_type;
    using Traits = typename StringType::traits_type;
    const TestData<CharType> data;
    const CharType nul = StorageType::nul();

    // 1. empty string, const
    {
        const StringType s;
        ASSERT_NE(s.c_str(), nullptr);
        ASSERT_NE(s.data(), nullptr);
        ASSERT_EQ(s.c_str(), s.data());
        ASSERT_TRUE(Traits::compare(s.c_str(), data.empty, 1) == 0);
        ASSERT_EQ(s[0], nul);
        // at() will throw
        ASSERT_THROW(s.at(0), std::out_of_range);
        ASSERT_THROW(s.at(1), std::out_of_range);
        // front() and back() won't throw, but back() isn't allowed here
        ASSERT_EQ(s.front(), nul);
    }

    // 2. empty string, non-const
    {
        StringType s;
        ASSERT_NE(s.c_str(), nullptr);
        ASSERT_NE(s.data(), nullptr);
        ASSERT_EQ(s.c_str(), s.data());
        ASSERT_TRUE(Traits::compare(s.c_str(), data.empty, 1) == 0);
        ASSERT_EQ(s[0], nul);
        // at() will throw
        ASSERT_THROW(s.at(0), std::out_of_range);
        ASSERT_THROW(s.at(1), std::out_of_range);
        // front() and back() won't throw, but back() isn't allowed here
        ASSERT_EQ(s.front(), nul);
    }

    // 3. non-empty string, const
    {
        const StringType s(data.hello_world);
        ASSERT_NE(s.c_str(), nullptr);
        ASSERT_NE(s.data(), nullptr);
        ASSERT_EQ(s.c_str(), s.data());
        ASSERT_TRUE(Traits::compare(s.c_str(), data.hello_world, data.hello_world_len) == 0);
        ASSERT_EQ(s.size(), data.hello_world_len);
        ASSERT_EQ(s.length(), data.hello_world_len);
        // note: comparision includes NUL
        for (size_t i = 0; i <= s.size(); ++i)
            ASSERT_EQ(s[i], data.hello_world[i]);
        for (size_t i = 0; i < s.size(); ++i)
            ASSERT_EQ(s.at(i), data.hello_world[i]);
        // at() will throw if pos >= size()
        ASSERT_THROW(s.at(data.hello_world_len), std::out_of_range);
        ASSERT_THROW(s.at(data.hello_world_len + 47), std::out_of_range);

        ASSERT_EQ(s.front(), data.hello_world[0]);
        ASSERT_EQ(s.back(), data.hello_world[data.hello_world_len - 1]);
    }

    // 4. non-empty string, non-const
    {
        StringType s(data.hello_world);
        ASSERT_NE(s.c_str(), nullptr);
        ASSERT_NE(s.data(), nullptr);
        ASSERT_EQ(s.c_str(), s.data());
        ASSERT_TRUE(Traits::compare(s.c_str(), data.hello_world, data.hello_world_len) == 0);
        ASSERT_EQ(s.size(), data.hello_world_len);
        ASSERT_EQ(s.length(), data.hello_world_len);
        // note: comparision includes NUL
        for (size_t i = 0; i <= s.size(); ++i)
            ASSERT_EQ(s[i], data.hello_world[i]);
        for (size_t i = 0; i < s.size(); ++i)
            ASSERT_EQ(s.at(i), data.hello_world[i]);
        // at() will throw if pos >= size()
        ASSERT_THROW(s.at(data.hello_world_len), std::out_of_range);
        ASSERT_THROW(s.at(data.hello_world_len + 47), std::out_of_range);

        ASSERT_EQ(s.front(), data.hello_world[0]);
        ASSERT_EQ(s.back(), data.hello_world[data.hello_world_len - 1]);

        // here, data() is writable
        s.data()[0] = nul;
        ASSERT_FALSE(Traits::compare(s.c_str(), data.hello_world, data.hello_world_len) == 0);
        ASSERT_EQ(Traits::length(s.data()), 0u);
        ASSERT_EQ(Traits::length(s.c_str()), 0u);
    }
}

/* iterator functions */
TYPED_TEST(StringCoreTest, IteratorFunctions)
{
    using StringType = TypeParam; // gtest specific
    using StorageType = typename StringType::storage_type;
    using CharType = typename StorageType::char_type;
    const TestData<CharType> data;

    // 1. constant
    {
        const StringType s(data.hello_world);

        // begin() + end()
        size_t i = 0;
        for (auto it = s.begin(); it != s.end(); ++it)
            ASSERT_EQ(*it, data.hello_world[i++]);

        // cbegin() + cend()
        i = 0;
        for (auto it = s.cbegin(); it != s.cend(); ++it)
            ASSERT_EQ(*it, data.hello_world[i++]);

        // reverse tests - create a reference string for comparisons
        const std::basic_string<CharType> hw(data.hello_world);
        {
            // rbegin() + rend()
            auto it1 = s.rbegin();
            auto it2 = hw.rbegin();
            while (it1 != s.rend() && it2 != hw.rend())
            {
                ASSERT_EQ(*it1, *it2);
                ++it1;
                ++it2;
            }
            ASSERT_TRUE(it1 == s.rend());
            ASSERT_TRUE(it2 == hw.rend());
        }
        {
            // crbegin() + crend()
            auto it1 = s.crbegin();
            auto it2 = hw.crbegin();
            while (it1 != s.crend() && it2 != hw.crend())
            {
                ASSERT_EQ(*it1, *it2);
                ++it1;
                ++it2;
            }
            ASSERT_TRUE(it1 == s.crend());
            ASSERT_TRUE(it2 == hw.crend());
        }
    }

    // 2. non-constant
    {
        StringType s(data.hello_world);

        // begin() + end()
        size_t i = 0;
        for (auto it = s.begin(); it != s.end(); ++it)
            ASSERT_EQ(*it, data.hello_world[i++]);

        // cbegin() + cend()
        i = 0;
        for (auto it = s.cbegin(); it != s.cend(); ++it)
            ASSERT_EQ(*it, data.hello_world[i++]);

        // iterators are non-const
        *s.begin() = *s.cbegin();
        *s.rbegin() = *s.crbegin();

        // reverse tests - create a reference string for comparisons
        std::basic_string<CharType> hw(data.hello_world);
        {
            // rbegin() + rend()
            auto it1 = s.rbegin();
            auto it2 = hw.rbegin();
            while (it1 != s.rend() && it2 != hw.rend())
            {
                ASSERT_EQ(*it1, *it2);
                ++it1;
                ++it2;
            }
            ASSERT_TRUE(it1 == s.rend());
            ASSERT_TRUE(it2 == hw.rend());
        }
        {
            // crbegin() + crend()
            auto it1 = s.crbegin();
            auto it2 = hw.crbegin();
            while (it1 != s.crend() && it2 != hw.crend())
            {
                ASSERT_EQ(*it1, *it2);
                ++it1;
                ++it2;
            }
            ASSERT_TRUE(it1 == s.crend());
            ASSERT_TRUE(it2 == hw.crend());
        }
    }
}


/* capacity functions */
TYPED_TEST(StringCoreTest, CapacityFunctions)
{
    using StringType = TypeParam; // gtest specific
    using StorageType = typename StringType::storage_type;
    using CharType = typename StorageType::char_type;
    const TestData<CharType> data;

    StringType s;
    ASSERT_TRUE(s.empty());
    ASSERT_EQ(s.size(), 0u);
    ASSERT_EQ(s.length(), 0u);
    ASSERT_GT(s.max_size(), 0u);
    ASSERT_GE(s.capacity(), 0u);
    // note: max is 64
    s.reserve(57);
    ASSERT_GE(s.capacity(), 57u);
    // no general test, only call it...
    s.shrink_to_fit();

    s = data.blablabla;
    ASSERT_FALSE(s.empty());
    ASSERT_EQ(s.size(), data.blablabla_len);
    ASSERT_EQ(s.length(), data.blablabla_len);
    ASSERT_GE(s.max_size(), data.blablabla_len);
    ASSERT_GE(s.capacity(), data.blablabla_len);
    // note: max is 64
    ASSERT_GE(57u, data.blablabla_len);
    s.reserve(57);
    ASSERT_GE(s.capacity(), 57u);
    // no general test, only call it...
    s.shrink_to_fit();
    ASSERT_TRUE(s == data.blablabla);
}


/* operations */
TYPED_TEST(StringCoreTest, Operations)
{
    using StringType = TypeParam; // gtest specific
    using StorageType = typename StringType::storage_type;
    using CharType = typename StorageType::char_type;
    using Traits = typename StringType::traits_type;
    const TestData<CharType> data;
    const CharType nul = StorageType::nul();

    StringType s1;
    const CharType ch = data.hello_world[3];

    // clear
    s1 = data.hello_world;
    ASSERT_FALSE(s1.empty());
    s1.clear();
    ASSERT_TRUE(s1.empty());

    // push_back
    const size_t num = 20;
    for (size_t i = 0; i < num; ++i)
        s1.push_back(ch);
    ASSERT_EQ(s1.size(), num);
    for (size_t i = 0; i < num; ++i)
        ASSERT_EQ(s1[i], ch);
    ASSERT_EQ(s1[num], nul);

    // pop_back
    s1 = data.hello_world;
    ASSERT_FALSE(s1.empty());
    size_t numPops = 0;
    while (!s1.empty())
    {
        s1.pop_back();
        ++numPops;
        ASSERT_EQ(s1.size(), data.hello_world_len - numPops);
        ASSERT_TRUE(Traits::compare(s1.c_str(), data.hello_world, s1.length()) == 0);
        const std::basic_string<CharType> ref(data.hello_world, s1.length());
        ASSERT_TRUE(s1 == ref);
    }
    ASSERT_EQ(numPops, data.hello_world_len);
    ASSERT_TRUE(s1.empty());

    // substr (const and non-const)
    const StringType s2(data.hello_world);
    s1 = s2;
    const std::basic_string<CharType> ref(data.hello_world);
    for (size_t pos = 0; pos <= data.hello_world_len; ++pos)
    {
        for (size_t len = 0; len <= data.hello_world_len - pos; ++len)
        {
            auto subRef = ref.substr(pos, len);
            auto sub1 = s1.substr(pos, len);
            auto sub2 = s2.substr(pos, len);
            ASSERT_TRUE(sub1 == subRef);
            ASSERT_TRUE(sub2 == subRef);
            ASSERT_EQ(sub1.size(), len);
            ASSERT_EQ(sub2.size(), len);
        }

        // again with length = npos
        {
            auto subRef = ref.substr(pos, std::basic_string<CharType>::npos);
            auto sub1 = s1.substr(pos, StringType::npos);
            auto sub2 = s2.substr(pos, StringType::npos);
            ASSERT_TRUE(sub1 == subRef);
            ASSERT_TRUE(sub2 == subRef);
            ASSERT_EQ(sub1.size(), data.hello_world_len - pos);
            ASSERT_EQ(sub2.size(), data.hello_world_len - pos);
        }
        // again without length (so it defaults to npos again)
        {
            auto subRef = ref.substr(pos);
            auto sub1 = s1.substr(pos);
            auto sub2 = s2.substr(pos);
            ASSERT_TRUE(sub1 == subRef);
            ASSERT_TRUE(sub2 == subRef);
            ASSERT_EQ(sub1.size(), data.hello_world_len - pos);
            ASSERT_EQ(sub2.size(), data.hello_world_len - pos);
        }
    }
    // substr throws if pos > size()
    ASSERT_THROW(s1.substr(s1.size() + 1), std::out_of_range);
    ASSERT_THROW(s2.substr(s2.size() + 1), std::out_of_range);

    // copy
    CharType buffer[256];
    for (size_t pos = 0; pos <= data.hello_world_len; ++pos)
    {
        // + 10 because we are allowed to ask for more
        for (size_t len = 0; len <= data.hello_world_len - pos + 10; ++len)
        {
            ASSERT_TRUE(s1 == data.hello_world);
            size_t n1 = s1.copy(buffer, len, pos);
            // unchanged
            ASSERT_TRUE(s1 == data.hello_world);

            // we can get only as much as there is left...
            ASSERT_EQ(n1, std::min(len, data.hello_world_len - pos));
            ASSERT_TRUE(Traits::compare(buffer, data.hello_world + pos, n1) == 0);
        }
    }
    // copy throws if pos > size()
    ASSERT_THROW(s1.copy(buffer, 0, s1.size() + 1), std::out_of_range);
    ASSERT_THROW(s2.copy(buffer, 0, s2.size() + 1), std::out_of_range);

    // resize
    ASSERT_TRUE(s1 == data.hello_world);
    ASSERT_EQ(s1.size(), data.hello_world_len);
    // add a NUL character
    s1.resize(s1.size() + 1);
    ASSERT_EQ(s1.size(), data.hello_world_len + 1);
    ASSERT_TRUE(Traits::compare(s1.c_str(), data.hello_world, data.hello_world_len) == 0);
    ASSERT_EQ(s1[s1.size()], nul);
    ASSERT_EQ(s1[s1.size() - 1], nul);
    // remove that character again
    s1.resize(data.hello_world_len);
    ASSERT_TRUE(s1 == data.hello_world);
    ASSERT_EQ(s1.size(), data.hello_world_len);
    // shrink tests
    for (size_t i = data.hello_world_len; i != static_cast<size_t>(-1); --i)
    {
        s1.resize(i);
        ASSERT_EQ(s1.size(), i);
        ASSERT_EQ(s1.length(), i);
        ASSERT_TRUE(Traits::compare(s1.c_str(), data.hello_world, i) == 0);
    }
    ASSERT_TRUE(s1.empty());
    // grow tests
    for (size_t i = 1; i <= data.hello_world_len; ++i)
    {
        s1.resize(i, data.hello_world[i - 1]);
        ASSERT_EQ(s1.size(), i);
        ASSERT_EQ(s1.length(), i);
        ASSERT_TRUE(Traits::compare(s1.c_str(), data.hello_world, i) == 0);
    }

    // swap
    StringType s3(data.blablabla);
    s1.clear();
    ASSERT_TRUE(s1.empty());
    ASSERT_FALSE(s3.empty());
    ASSERT_TRUE(s3 == data.blablabla);
    s1.swap(s3);
    ASSERT_TRUE(s3.empty());
    ASSERT_FALSE(s1.empty());
    ASSERT_TRUE(s1 == data.blablabla);
    s3 = data.hello_world;
    s3.swap(s1);
    ASSERT_FALSE(s1.empty());
    ASSERT_FALSE(s3.empty());
    ASSERT_TRUE(s1 == data.hello_world);
    ASSERT_TRUE(s3 == data.blablabla);
}
