/**
 * @file	Special Purpose Strings Library: test_stringcore.cpp
 * @author	Daniel Evers
 * @brief	StringCore unit tests: various functions
 * @license MIT
 */

#include <gtest/gtest.h>

#include "spsl.hpp"
#include "testdata.hpp"
#include "tests.hpp"

/* comparison functions */
TYPED_TEST(StringCoreTest, ComparisonFunctions)
{
    using StringType = TypeParam; // gtest specific
    using StorageType = typename StringType::storage_type;
    using CharType = typename StorageType::char_type;
    const TestData<CharType> data;

    const StringType s(data.hello_world);
    StringType ref(data.hello_world);
    // note: if the prefix is identical, comparing with a shorter string yields rc > 0

    // char_type*
    ASSERT_EQ(s.compare(ref.c_str()), 0) << hexdump(s.c_str(), s.size()) << "\n"
                                         << hexdump(ref.c_str(), ref.size());
    ++ref[0];
    ASSERT_LT(s.compare(ref.c_str()), 0);
    ref[0] = static_cast<CharType>(ref[0] - 2);
    ASSERT_GT(s.compare(ref.c_str()), 0);
    ref = s.c_str();
    ref.pop_back();
    ASSERT_GT(s.compare(ref.c_str()), 0);
    ref = s.c_str();
    ref += s[0];
    ASSERT_LT(s.compare(ref.c_str()), 0);

    // pos, count, char_type*
    ref = s.c_str();
    ASSERT_EQ(s.compare(0, s.size(), ref.c_str()), 0);
    ASSERT_LT(s.compare(0, s.size() - 1, ref.c_str()), 0);
    if (s[0] > s[1])
    {
        ASSERT_LT(s.compare(1, s.size(), ref.c_str()), 0);
    }
    else if (s[0] < s[1])
    {
        ASSERT_GT(s.compare(1, s.size(), ref.c_str()), 0);
    }
    ASSERT_EQ(s.compare(1, s.size() - 1, ref.c_str() + 1), 0);
    ref.pop_back();
    ASSERT_GT(s.compare(0, s.size(), ref.c_str()), 0);

    // pos, count, char_type*, count
    ref = s.c_str();
    ASSERT_EQ(s.compare(0, s.size(), ref.c_str(), ref.size()), 0);
    ASSERT_EQ(s.compare(0, s.size() - 3, ref.c_str(), ref.size() - 3), 0);
    ASSERT_LT(s.compare(0, s.size() - 3, ref.c_str(), ref.size() - 2), 0);
    ASSERT_GT(s.compare(0, s.size() - 2, ref.c_str(), ref.size() - 3), 0);
    ASSERT_LT(s.compare(0, s.size() - 1, ref.c_str(), ref.size()), 0);
    if (s[0] > s[1])
    {
        ASSERT_LT(s.compare(1, s.size(), ref.c_str(), ref.size()), 0);
    }
    else if (s[0] < s[1])
    {
        ASSERT_GT(s.compare(1, s.size(), ref.c_str(), ref.size()), 0);
    }
    ASSERT_EQ(s.compare(1, s.size() - 1, ref.c_str() + 1, ref.size() - 1), 0);
    ref.pop_back();
    ASSERT_GT(s.compare(0, s.size(), ref.c_str(), ref.size()), 0);

    // other string class
    ref = s.c_str();
    ASSERT_EQ(s.compare(ref), 0);
    ++ref[0];
    ;
    ASSERT_LT(s.compare(ref), 0);
    ref[0] = static_cast<CharType>(ref[0] - 2);
    ASSERT_GT(s.compare(ref), 0);
    ref = s;
    ref.pop_back();
    ASSERT_GT(s.compare(ref), 0);
    ref = s;
    ref += s[0];
    ASSERT_LT(s.compare(ref), 0);

    // pos, count, string
    ref = s.c_str();
    ASSERT_EQ(s.compare(0, s.size(), ref), 0);
    ASSERT_LT(s.compare(0, s.size() - 1, ref), 0);
    if (s[0] > s[1])
    {
        ASSERT_LT(s.compare(1, s.size(), ref), 0);
    }
    else if (s[0] < s[1])
    {
        ASSERT_GT(s.compare(1, s.size(), ref), 0);
    }
    auto ref2 = ref.substr(1);
    ASSERT_EQ(s.compare(1, s.size() - 1, ref2), 0);
    ref.pop_back();
    ASSERT_GT(s.compare(0, s.size(), ref), 0);

    // pos, count, string, pos, count
    ref = s.c_str();
    ASSERT_EQ(s.compare(0, s.size(), ref, 0, ref.size()), 0);
    ASSERT_EQ(s.compare(5, s.size() - 5, ref, 5, ref.size() - 5), 0);
    ASSERT_EQ(s.compare(0, s.size() - 3, ref, 0, ref.size() - 3), 0);
    ASSERT_LT(s.compare(0, s.size() - 3, ref, 0, ref.size() - 2), 0);
    ASSERT_GT(s.compare(0, s.size() - 2, ref, 0, ref.size() - 3), 0);
    ASSERT_LT(s.compare(0, s.size() - 1, ref, 0, ref.size()), 0);
    if (s[0] > s[1])
    {
        ASSERT_LT(s.compare(1, s.size(), ref, 0, ref.size()), 0);
        ASSERT_GT(s.compare(0, s.size(), ref, 1, ref.size()), 0);
    }
    else if (s[0] < s[1])
    {
        ASSERT_GT(s.compare(1, s.size(), ref, 0, ref.size()), 0);
        ASSERT_LT(s.compare(0, s.size(), ref, 1, ref.size()), 0);
    }
    ASSERT_EQ(s.compare(1, s.size() - 1, ref, 1, ref.size() - 1), 0);
    ASSERT_GT(s.compare(0, s.size(), ref, 0, ref.size() - 1), 0);
}

/* find functions */
TYPED_TEST(StringCoreTest, FindFunctions)
{
    using StringType = TypeParam; // gtest specific
    using StorageType = typename StringType::storage_type;
    using CharType = typename StorageType::char_type;
    using Traits = typename StringType::traits_type;
    const TestData<CharType> data;
    const auto npos = StringType::npos;
    const CharType nul = StorageType::nul();

    const StringType s(data.hello_world);
    const CharType H = data.hello_world[0];
    const CharType W = data.hello_world[6];
    const CharType b = data.blablabla[0];
    const CharType* world = data.hello_world + 6;
    const auto world_len = Traits::length(world);

    // find: char_type*, pos, count
    ASSERT_EQ(s.find(s.data(), 0, s.size()), 0u);
    ASSERT_EQ(s.find(world, 0, world_len), 6u);
    ASSERT_EQ(s.find(world, 0, 1), 6u);
    ASSERT_EQ(s.find(world, 7, world_len), npos);
    // searching for an empty string finds the current position (if valid)
    ASSERT_EQ(s.find(world, 0, 0), 0u);
    ASSERT_EQ(s.find(world, 3, 0), 3u);
    ASSERT_EQ(s.find(world, s.size(), 0), s.size());
    ASSERT_EQ(s.find(world, s.size() + 1, 0), npos);

    // find: char_type, pos
    ASSERT_EQ(s.find(b, 0), npos);
    ASSERT_EQ(s.find(W), 6u);
    ASSERT_EQ(s.find(H), 0u);
    ASSERT_EQ(s.find(H, 1), npos);

    // find: char_type*, pos
    ASSERT_EQ(s.find(s.data()), 0u);
    ASSERT_EQ(s.find(s.data(), 0), 0u);
    ASSERT_EQ(s.find(world), 6u);
    ASSERT_EQ(s.find(world, 0), 6u);
    ASSERT_EQ(s.find(world, 7), npos);
    // searching for an empty string finds the current position (if valid)
    {
        const CharType empty[1] = { nul };
        ASSERT_EQ(s.find(empty, 0, 0), 0u);
        ASSERT_EQ(s.find(empty, 3, 0), 3u);
        ASSERT_EQ(s.find(empty, s.size(), 0), s.size());
        ASSERT_EQ(s.find(empty, s.size() + 1, 0), npos);
    }

    // find: String, pos
    const std::basic_string<CharType> ref(world);
    ASSERT_EQ(s.find(s), 0u);
    ASSERT_EQ(s.find(s, 0), 0u);
    ASSERT_EQ(s.find(ref), 6u);
    ASSERT_EQ(s.find(ref, 0), 6u);
    ASSERT_EQ(s.find(ref, 7), npos);
    // searching for an empty string finds the current position (if valid)
    {
        const std::basic_string<CharType> empty;
        ASSERT_EQ(s.find(empty), 0u);
        ASSERT_EQ(s.find(empty, 3), 3u);
        ASSERT_EQ(s.find(empty, s.size()), s.size());
        ASSERT_EQ(s.find(empty, s.size() + 1), npos);
    }

    // rfind: char_type*, pos, count
    ASSERT_EQ(s.rfind(s.data(), s.size() - 1, s.size()), 0u);
    ASSERT_EQ(s.rfind(s.data(), npos, s.size()), 0u);
    ASSERT_EQ(s.rfind(world, npos, world_len), 6u);
    ASSERT_EQ(s.rfind(world, npos, 1), 6u);
    ASSERT_EQ(s.rfind(world, 5, world_len), npos);

    // rfind: char_type, pos
    ASSERT_EQ(s.rfind(b, npos), npos);
    ASSERT_EQ(s.rfind(W), 6u);
    ASSERT_EQ(s.rfind(H), 0u);
    ASSERT_EQ(s.rfind(W, 5), npos);

    // rfind: char_type*, pos
    ASSERT_EQ(s.rfind(s.data()), 0u);
    ASSERT_EQ(s.rfind(s.data(), s.size() - 1), 0u);
    ASSERT_EQ(s.rfind(s.data(), npos), 0u);
    ASSERT_EQ(s.rfind(world), 6u);
    ASSERT_EQ(s.rfind(world, npos), 6u);
    ASSERT_EQ(s.rfind(world, 5), npos);

    // rfind: String, pos
    ASSERT_EQ(s.rfind(s), 0u);
    ASSERT_EQ(s.rfind(s, s.size() - 1), 0u);
    ASSERT_EQ(s.rfind(s, npos), 0u);
    ASSERT_EQ(s.rfind(ref), 6u);
    ASSERT_EQ(s.rfind(ref, s.size() - 1), 6u);
    ASSERT_EQ(s.rfind(ref, npos), 6u);
    ASSERT_EQ(s.rfind(ref, 5), npos);
}

/* comparison operators */
TYPED_TEST(StringCoreTest, ComparisonOperators)
{
    using StringType = TypeParam; // gtest specific
    using StorageType = typename StringType::storage_type;
    using CharType = typename StorageType::char_type;
    const TestData<CharType> data;

    const StringType s(data.hello_world);
    const StringType bla(data.blablabla);
    const StringType hello(data.hello_world);
    std::basic_string<CharType> ref;

    // 1. StringCore, char_type*
    ASSERT_TRUE(s == data.hello_world) << hexdump(s.data(), s.size() * sizeof(CharType));
    ASSERT_TRUE(s != data.blablabla);
    ASSERT_FALSE(s == data.blablabla);
    ASSERT_TRUE(s <= data.hello_world);
    ASSERT_TRUE(s >= data.hello_world);
    ASSERT_FALSE(s < data.hello_world);
    ASSERT_FALSE(s > data.hello_world);
    ref = data.hello_world;
    --ref[0];
    ASSERT_TRUE(s > ref.c_str());
    ASSERT_TRUE(s >= ref.c_str());
    ASSERT_FALSE(s < ref.c_str());
    ASSERT_FALSE(s <= ref.c_str());
    ref[0] = static_cast<CharType>(ref[0] + 2);
    ASSERT_TRUE(s < ref.c_str());
    ASSERT_TRUE(s <= ref.c_str());
    ASSERT_FALSE(s > ref.c_str());
    ASSERT_FALSE(s >= ref.c_str());

    // 2. char_type*, StringCore
    ASSERT_TRUE(data.hello_world == s);
    ASSERT_TRUE(data.blablabla != s);
    ASSERT_FALSE(data.blablabla == s);
    ASSERT_TRUE(data.hello_world >= s);
    ASSERT_TRUE(data.hello_world <= s);
    ASSERT_FALSE(data.hello_world > s);
    ASSERT_FALSE(data.hello_world < s);
    ref = data.hello_world;
    --ref[0];
    ASSERT_TRUE(ref.c_str() < s);
    ASSERT_TRUE(ref.c_str() <= s);
    ASSERT_FALSE(ref.c_str() > s);
    ASSERT_FALSE(ref.c_str() >= s);
    ref[0] = static_cast<CharType>(ref[0] + 2);
    ASSERT_TRUE(ref.c_str() > s);
    ASSERT_TRUE(ref.c_str() >= s);
    ASSERT_FALSE(ref.c_str() < s);
    ASSERT_FALSE(ref.c_str() <= s);

    // 3. StringCore, StringCore
    ASSERT_TRUE(s == s);
    ASSERT_FALSE(s != s);
    ASSERT_TRUE(s == hello);
    ASSERT_FALSE(s != hello);
    ASSERT_FALSE(s == bla);
    ASSERT_TRUE(s != bla);
    ASSERT_TRUE(s <= hello);
    ASSERT_TRUE(s >= hello);
    ASSERT_FALSE(s < hello);
    ASSERT_FALSE(s > hello);
    StringType ref2 = data.hello_world;
    --ref2[0];
    ASSERT_TRUE(s > ref2);
    ASSERT_TRUE(s >= ref2);
    ASSERT_FALSE(s < ref2);
    ASSERT_FALSE(s <= ref2);
    ref2[0] = static_cast<CharType>(ref2[0] + 2);
    ASSERT_TRUE(s < ref2);
    ASSERT_TRUE(s <= ref2);
    ASSERT_FALSE(s > ref2);
    ASSERT_FALSE(s >= ref2);

    // 4. StringCore, other string class
    ref = s.c_str();
    ASSERT_TRUE(s == ref);
    ASSERT_FALSE(s != ref);
    ASSERT_TRUE(s == ref);
    ASSERT_FALSE(s != ref);
    ref = data.blablabla;
    ASSERT_FALSE(s == ref);
    ASSERT_TRUE(s != ref);
    ref = s.c_str();
    ASSERT_TRUE(s <= ref);
    ASSERT_TRUE(s >= ref);
    ASSERT_FALSE(s < ref);
    ASSERT_FALSE(s > ref);
    --ref[0];
    ASSERT_TRUE(s > ref);
    ASSERT_TRUE(s >= ref);
    ASSERT_FALSE(s < ref);
    ASSERT_FALSE(s <= ref);
    ref[0] = static_cast<CharType>(ref[0] + 2);
    ASSERT_TRUE(s < ref);
    ASSERT_TRUE(s <= ref);
    ASSERT_FALSE(s > ref);
    ASSERT_FALSE(s >= ref);

    // 5. other string class, StringCore
    ref = s.c_str();
    ASSERT_TRUE(ref == s);
    ASSERT_FALSE(ref != s);
    ASSERT_TRUE(ref == s);
    ASSERT_FALSE(ref != s);
    ref = data.blablabla;
    ASSERT_FALSE(ref == s);
    ASSERT_TRUE(ref != s);
    ref = s.c_str();
    ASSERT_TRUE(ref <= s);
    ASSERT_TRUE(ref >= s);
    ASSERT_FALSE(ref < s);
    ASSERT_FALSE(ref > s);
    --ref[0];
    ASSERT_TRUE(ref < s);
    ASSERT_TRUE(ref <= s);
    ASSERT_FALSE(ref > s);
    ASSERT_FALSE(ref >= s);
    ref[0] = static_cast<CharType>(ref[0] + 2);
    ASSERT_TRUE(ref > s);
    ASSERT_TRUE(ref >= s);
    ASSERT_FALSE(ref < s);
    ASSERT_FALSE(ref <= s);
}


/* swap implementation */
TYPED_TEST(StringCoreTest, SwapSpecialization)
{
    using StringType = TypeParam; // gtest specific
    using StorageType = typename StringType::storage_type;
    using CharType = typename StorageType::char_type;
    const TestData<CharType> data;

    StringType s1;

    // swap
    StringType s2(data.blablabla);
    ASSERT_TRUE(s1.empty());
    ASSERT_FALSE(s2.empty());
    ASSERT_TRUE(s2 == data.blablabla) << hexdump(s2.c_str(), s2.size()) << "\n"
                                      << hexdump(data.blablabla, data.blablabla_len);

    swap(s1, s2);
    ASSERT_TRUE(s2.empty());
    ASSERT_FALSE(s1.empty());
    ASSERT_TRUE(s1 == data.blablabla);
    s2 = data.hello_world;
    swap(s2, s1);
    ASSERT_FALSE(s1.empty());
    ASSERT_FALSE(s2.empty());
    ASSERT_TRUE(s1 == data.hello_world);
    ASSERT_TRUE(s2 == data.blablabla);
}


/* hash specialization */
TYPED_TEST(StringCoreTest, HashSpecialization)
{
    using StringType = TypeParam; // gtest specific
    using StorageType = typename StringType::storage_type;
    using CharType = typename StorageType::char_type;
    const TestData<CharType> data;

    StringType s1;
    std::hash<StringType> hash;

    // hash an empty string
    std::size_t val1 = hash(s1);
    std::size_t val2 = hash(s1);
    ASSERT_EQ(val1, val2);

    // hash a non-empty string - should have a different value
    s1 = data.hello_world;
    std::size_t val3 = hash(s1);
    std::size_t val4 = hash(s1);
    ASSERT_EQ(val3, val4);
    ASSERT_NE(val1, val3);

    // hash another non-empty string - should have a different value
    s1 = data.blablabla;
    std::size_t val5 = hash(s1);
    std::size_t val6 = hash(s1);
    ASSERT_EQ(val5, val6);
    ASSERT_NE(val1, val5);
    ASSERT_NE(val3, val5);
}
