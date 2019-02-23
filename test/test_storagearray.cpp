/**
 * @file	Special Purpose Strings Library: test_storagearray.cpp
 * @author	Daniel Evers
 * @brief   StorageArray unit tests
 * @license MIT
 */

#include <gtest/gtest.h>

#include "spsl/storage_array.hpp"
#include "testdata.hpp"

template <typename CharType>
class StorageArrayTest : public ::testing::Test
{
};

// all character types we want to test
using CharTypes = testing::Types<char, wchar_t>;


TYPED_TEST_SUITE(StorageArrayTest, CharTypes);

/* check if size is checkable on type at run/compile time */
TYPED_TEST(StorageArrayTest, StaticSize)
{
    using CharType = TypeParam; // gtest specific
    using ArrayType = spsl::StorageArray<CharType, 64>;
    using size_type = typename ArrayType::size_type;

    ASSERT_EQ(ArrayType::max_size(), 64u);
#ifdef SPSL_HAS_NOEXCEPT
    static_assert(ArrayType::max_size() == 64,
                  "Size of StorageArray has to be available at compile time");
#endif

    // and check the size requirements (there may be some padding, but max. 1 word)
    constexpr size_t minSize = sizeof(size_type) + sizeof(CharType) * (ArrayType::max_size() + 1);
#if defined(__MINGW32__) || defined(__MINGW64__)
    // not sure why, but MinGW needs a few more bytes...
    constexpr size_t maxSize = minSize + 2 * sizeof(long);
#else
    constexpr size_t maxSize = minSize + sizeof(long);
#endif
    ASSERT_LE(minSize, sizeof(ArrayType));
    ASSERT_GE(maxSize, sizeof(ArrayType));
    //   static_assert(minSize <= sizeof(ArrayType) && sizeof(ArrayType) <= maxSize,
    //                 "Unexpected object size?");
}

/* constructor tests */
TYPED_TEST(StorageArrayTest, ConstructorTests)
{
    using CharType = TypeParam; // gtest specific
    using ArrayType = spsl::StorageArray<CharType, 64>;
    const CharType nul = ArrayType::nul;

    const ArrayType s1;
    ASSERT_EQ(s1.capacity(), 64u);
    ASSERT_EQ(s1.max_size(), 64u);

    ASSERT_TRUE(s1.empty());
    ASSERT_EQ(s1.length(), 0u);
    ASSERT_EQ(s1.length(), s1.size());
    ASSERT_EQ(s1.capacity_left(), s1.max_size());
    // valid content?
    ASSERT_NE(s1.data(), nullptr);
    ASSERT_EQ(s1.data()[0], nul);

    // copy & move constructor are available
    ArrayType s2(s1);
    ArrayType s3(std::move(s2));
}

/* assignment functions */
TYPED_TEST(StorageArrayTest, AssignmentTests)
{
    using CharType = TypeParam; // gtest specific
    using ArrayType = spsl::StorageArray<CharType, 64>;
    const TestData<CharType> data{};
    using Traits = typename ArrayType::traits_type;

    ArrayType arr;
    // assign a string
    arr.assign(data.hello_world, data.hello_world_len);
    ASSERT_EQ(arr.length(), Traits::length(arr.data()));
    ASSERT_EQ(arr.length(), data.hello_world_len);
    ASSERT_TRUE(Traits::compare(arr.data(), data.hello_world, data.hello_world_len) == 0);

    // assign something else
    arr.assign(data.blablabla, data.blablabla_len);
    ASSERT_EQ(arr.length(), Traits::length(arr.data()));
    ASSERT_EQ(arr.length(), data.blablabla_len);
    ASSERT_TRUE(Traits::compare(arr.data(), data.blablabla, data.blablabla_len) == 0);

    // assign a repeated character
    const CharType ch = data.hello_world[0];
    arr.assign(33, ch);
    ASSERT_EQ(arr.length(), 33u);
    for (size_t i = 0; i < 33; ++i)
    {
        ASSERT_EQ(arr[i], ch);
    }

    // assign an iterator range - we use std::basic_string here
    const std::basic_string<CharType> bs(data.hello_world);
    arr.assign(bs.begin(), bs.end());
    ASSERT_EQ(arr.length(), bs.length());
    ASSERT_TRUE(Traits::compare(arr.data(), data.hello_world, data.hello_world_len) == 0);
    ASSERT_TRUE(bs == arr.data());
}

/* push_back/pop_back functions */
TYPED_TEST(StorageArrayTest, PushPopTests)
{
    using CharType = TypeParam; // gtest specific
    using ArrayType = spsl::StorageArray<CharType, 64>;
    const TestData<CharType> data{};
    using Traits = typename ArrayType::traits_type;

    ArrayType arr;
    // append a string byte for byte
    for (size_t i = 0; i < data.hello_world_len; ++i)
    {
        ASSERT_EQ(arr.length(), i);
        arr.push_back(data.hello_world[i]);
        ASSERT_EQ(arr.length(), i + 1);
    }
    ASSERT_EQ(arr.length(), data.hello_world_len);
    ASSERT_TRUE(Traits::compare(arr.data(), data.hello_world, data.hello_world_len) == 0);

    // remove the last byte
    for (size_t i = 0; i < data.hello_world_len; ++i)
    {
        ASSERT_EQ(arr.length(), data.hello_world_len - i);
        arr.pop_back();
        ASSERT_EQ(arr.length(), data.hello_world_len - i - 1);
    }
    ASSERT_EQ(arr.length(), 0u);
    ASSERT_TRUE(arr.empty());
}

/* insert functions */
TYPED_TEST(StorageArrayTest, InsertTests)
{
    using CharType = TypeParam; // gtest specific
    using ArrayType = spsl::StorageArray<CharType, 64>;
    const TestData<CharType> data{};
    using Traits = typename ArrayType::traits_type;

    // prototype: void insert(size_type index, size_type count, char_type ch)
    ArrayType arr;
    const CharType ch = data.hello_world[0];
    arr.insert(0, 5, ch);
    ASSERT_EQ(arr.length(), 5u);
    for (size_t i = 0; i < 5; ++i)
        ASSERT_EQ(arr[i], ch);

    // insert again at the beginning
    const CharType ch2 = data.hello_world[1];
    arr.insert(0, 5, ch2);
    ASSERT_EQ(arr.length(), 10u);
    for (size_t i = 0; i < 5; ++i)
        ASSERT_EQ(arr[i], ch2);
    for (size_t i = 5; i < 10; ++i)
        ASSERT_EQ(arr[i], ch);

    // insert in the middle
    const CharType ch3 = data.hello_world[2];
    arr.insert(5, 10, ch3);
    ASSERT_EQ(arr.length(), 20u);
    for (size_t i = 0; i < 5; ++i)
        ASSERT_EQ(arr[i], ch2);
    for (size_t i = 5; i < 15; ++i)
        ASSERT_EQ(arr[i], ch3);
    for (size_t i = 15; i < 20; ++i)
        ASSERT_EQ(arr[i], ch);

    const std::basic_string<CharType> bs(data.blablabla);

    // insert a string
    ArrayType arr2(arr);
    arr.insert(15, bs.data(), bs.size());
    ASSERT_EQ(arr.length(), 20 + bs.length());

    for (size_t i = 0; i < 5; ++i)
        ASSERT_EQ(arr[i], ch2);
    for (size_t i = 5; i < 15; ++i)
        ASSERT_EQ(arr[i], ch3);
    ASSERT_TRUE(Traits::compare(arr.data() + 15, data.blablabla, data.blablabla_len) == 0);
    for (size_t i = 15 + data.blablabla_len; i < 20 + data.blablabla_len; ++i)
        ASSERT_EQ(arr[i], ch);

    // again using an iterator range
    arr = arr2;
    arr.insert(15, bs.begin(), bs.end());
    ASSERT_EQ(arr.length(), 20 + bs.length());

    for (size_t i = 0; i < 5; ++i)
        ASSERT_EQ(arr[i], ch2);
    for (size_t i = 5; i < 15; ++i)
        ASSERT_EQ(arr[i], ch3);
    ASSERT_TRUE(Traits::compare(arr.data() + 15, data.blablabla, data.blablabla_len) == 0);
    for (size_t i = 15 + data.blablabla_len; i < 20 + data.blablabla_len; ++i)
        ASSERT_EQ(arr[i], ch);
}

/* append functions */
TYPED_TEST(StorageArrayTest, AppendTests)
{
    using CharType = TypeParam; // gtest specific
    using ArrayType = spsl::StorageArray<CharType, 64>;
    const TestData<CharType> data{};

    // apply all functions to the array and std::basic_string at the same time and make sure they
    // stay identical
    ArrayType arr;
    std::basic_string<CharType> ref;

    // void append(const char_type* s, size_type n)
    arr.append(data.hello_world, data.hello_world_len);
    ref.append(data.hello_world, data.hello_world_len);
    ASSERT_EQ(ref, arr.data());
    ASSERT_EQ(ref.length(), arr.length());
    ASSERT_EQ(ref.size(), arr.size());

    arr.append(data.blablabla, data.blablabla_len);
    ref.append(data.blablabla, data.blablabla_len);
    ASSERT_EQ(ref, arr.data());
    ASSERT_EQ(ref.length(), arr.length());
    ASSERT_EQ(ref.size(), arr.size());

    // void append(size_type count, char_type ch)
    const CharType ch = data.hello_world[0];
    arr.append(20, ch);
    ref.append(20, ch);
    ASSERT_EQ(ref, arr.data());
    ASSERT_EQ(ref.length(), arr.length());
    ASSERT_EQ(ref.size(), arr.size());

    // void append(InputIt first, InputIt last)
    const std::basic_string<CharType> bs(data.blablabla);
    arr.append(bs.begin(), bs.end());
    ref.append(bs.begin(), bs.end());
    ASSERT_EQ(ref, arr.data());
    ASSERT_EQ(ref.length(), arr.length());
    ASSERT_EQ(ref.size(), arr.size());
}

/* swap function */
TYPED_TEST(StorageArrayTest, SwapTests)
{
    using CharType = TypeParam; // gtest specific
    using ArrayType = spsl::StorageArray<CharType, 64>;
    const TestData<CharType> data{};
    using Traits = typename ArrayType::traits_type;

    ArrayType arr1, arr2;
    // let's start with swapping empty strings...
    ASSERT_TRUE(arr1.empty());
    ASSERT_TRUE(arr2.empty());
    arr1.swap(arr2);
    ASSERT_TRUE(arr1.empty());
    ASSERT_TRUE(arr2.empty());

    // assign + swap
    arr1.assign(data.hello_world, data.hello_world_len);
    ASSERT_EQ(arr1.length(), data.hello_world_len);
    ASSERT_TRUE(Traits::compare(arr1.data(), data.hello_world, data.hello_world_len) == 0);
    ASSERT_EQ(arr2.length(), 0u);
    arr1.swap(arr2);
    ASSERT_EQ(arr2.length(), data.hello_world_len);
    ASSERT_TRUE(Traits::compare(arr2.data(), data.hello_world, data.hello_world_len) == 0);
    ASSERT_EQ(arr1.length(), 0u);

    // swap back
    arr1.swap(arr2);
    ASSERT_EQ(arr1.length(), data.hello_world_len);
    ASSERT_TRUE(Traits::compare(arr1.data(), data.hello_world, data.hello_world_len) == 0);
    ASSERT_EQ(arr2.length(), 0u);

    // now swap 2 non-empty strings
    arr1.assign(data.hello_world, data.hello_world_len);
    ASSERT_EQ(arr1.length(), data.hello_world_len);
    ASSERT_TRUE(Traits::compare(arr1.data(), data.hello_world, data.hello_world_len) == 0);
    arr2.assign(data.blablabla, data.blablabla_len);
    ASSERT_EQ(arr2.length(), data.blablabla_len);
    ASSERT_TRUE(Traits::compare(arr2.data(), data.blablabla, data.blablabla_len) == 0);
    arr2.swap(arr1);
    ASSERT_EQ(arr2.length(), data.hello_world_len);
    ASSERT_TRUE(Traits::compare(arr2.data(), data.hello_world, data.hello_world_len) == 0);
    ASSERT_EQ(arr1.length(), data.blablabla_len);
    ASSERT_TRUE(Traits::compare(arr1.data(), data.blablabla, data.blablabla_len) == 0);
}

/* resize function */
TYPED_TEST(StorageArrayTest, ResizeTests)
{
    using CharType = TypeParam; // gtest specific
    using ArrayType = spsl::StorageArray<CharType, 64>;
    const TestData<CharType> data{};
    const CharType nul = ArrayType::nul;

    // we'll stay within max_size() here
    ArrayType arr;
    // empty
    ASSERT_EQ(arr.size(), 0u);
    ASSERT_EQ(arr.length(), 0u);
    ASSERT_EQ(arr.max_size(), 64u);
    ASSERT_EQ(arr.capacity_left(), 64u);
    const CharType ch = data.hello_world[0];

    // resize + 10 characters
    arr.resize(10, ch);
    ASSERT_EQ(arr.size(), 10u);
    ASSERT_EQ(arr.length(), 10u);
    ASSERT_EQ(arr.max_size(), 64u);
    ASSERT_EQ(arr.capacity_left(), 54u);
    for (size_t i = 0; i < 10; ++i)
        ASSERT_EQ(arr[i], ch);
    ASSERT_EQ(arr[10], nul);

    // again + 10 characters
    arr.resize(20, ch);
    ASSERT_EQ(arr.size(), 20u);
    ASSERT_EQ(arr.length(), 20u);
    ASSERT_EQ(arr.max_size(), 64u);
    ASSERT_EQ(arr.capacity_left(), 44u);
    for (size_t i = 0; i < 20; ++i)
        ASSERT_EQ(arr[i], ch);
    ASSERT_EQ(arr[20], nul);

    // shrink 3 characters
    arr.resize(17, ch);
    ASSERT_EQ(arr.size(), 17u);
    ASSERT_EQ(arr.length(), 17u);
    ASSERT_EQ(arr.max_size(), 64u);
    ASSERT_EQ(arr.capacity_left(), 47u);
    for (size_t i = 0; i < 17; ++i)
        ASSERT_EQ(arr[i], ch);
    ASSERT_EQ(arr[17], nul);

    // quick check: this no-op doesn't kill any kittens...
    arr.shrink_to_fit();
}

/* truncating when exceeding max_size() during assign() */
TYPED_TEST(StorageArrayTest, AssignTruncationTests)
{
    using CharType = TypeParam; // gtest specific
    // this variant definitely won't throw
    using ArrayType = spsl::StorageArray<CharType, 64, spsl::policy::overflow::Truncate>;
    const TestData<CharType> data{};

    // I can reserve as much as I want - it'll be ignored
    const size_t too_large = 1000;
    ArrayType arr;
    ASSERT_GT(too_large, arr.max_size());
    ASSERT_NO_THROW(arr.reserve(1000));

    // I can assign as much as I want - it'll be truncated
    const CharType ch = data.hello_world[0];
    ASSERT_NO_THROW(arr.assign(too_large, ch));
    ASSERT_EQ(arr.length(), arr.max_size());
    ASSERT_EQ(arr.size(), arr.max_size());
    ASSERT_EQ(arr.capacity_left(), 0u);
    for (size_t i = 0; i < arr.length(); ++i)
        ASSERT_EQ(arr[i], ch);

    // same test, but assign from a character array
    std::basic_string<CharType> ref;
    while (ref.size() < too_large)
        ref += data.hello_world;
    arr.clear();
    ASSERT_GE(ref.size(), too_large);
    ASSERT_GT(ref.size(), arr.max_size());
    ASSERT_NO_THROW(arr.assign(ref.data(), ref.size()));
    ASSERT_EQ(arr.length(), arr.max_size());
    ASSERT_EQ(arr.size(), arr.max_size());
    ASSERT_EQ(arr.capacity_left(), 0u);
    for (size_t i = 0; i < arr.length(); ++i)
        ASSERT_EQ(arr[i], ref[i]);

    // and again using an iterator
    arr.clear();
    ASSERT_GE(ref.size(), too_large);
    ASSERT_GT(ref.size(), arr.max_size());
    ASSERT_NO_THROW(arr.assign(ref.begin(), ref.end()));
    ASSERT_EQ(arr.length(), arr.max_size());
    ASSERT_EQ(arr.size(), arr.max_size());
    ASSERT_EQ(arr.capacity_left(), 0u);
    for (size_t i = 0; i < arr.length(); ++i)
        ASSERT_EQ(arr[i], ref[i]);

    // using push_back()
    arr.clear();
    for (auto c : ref)
    {
        ASSERT_NO_THROW(arr.push_back(c));
    }
    ASSERT_EQ(arr.length(), arr.max_size());
    ASSERT_EQ(arr.size(), arr.max_size());
    ASSERT_EQ(arr.capacity_left(), 0u);
    for (size_t i = 0; i < arr.length(); ++i)
        ASSERT_EQ(arr[i], ref[i]);
}

/* std::length_error when exceeding max_size() during assign() */
TYPED_TEST(StorageArrayTest, AssignLengthErrorTests)
{
    using CharType = TypeParam; // gtest specific
    // this variant definitely will throw
    using ArrayType = spsl::StorageArray<CharType, 64, spsl::policy::overflow::Throw>;
    const TestData<CharType> data{};

    ArrayType arr;
    const CharType ch = data.hello_world[0];
    arr.assign(3, ch);

    // I can't reserve as much as I want
    const size_t too_large = 1000;
    ASSERT_GT(too_large, arr.max_size());
    ASSERT_THROW(arr.reserve(1000), std::length_error);
    // the content is unchanged
    ASSERT_EQ(arr.length(), 3u);
    for (size_t i = 0; i < arr.length(); ++i)
        ASSERT_EQ(arr[i], ch);

    // I can't assign as much as I want
    ASSERT_THROW(arr.assign(too_large, ch), std::length_error);
    // the content is unchanged
    ASSERT_EQ(arr.length(), 3u);
    for (size_t i = 0; i < arr.length(); ++i)
        ASSERT_EQ(arr[i], ch);

    // same test, but assign from a character array
    std::basic_string<CharType> ref;
    while (ref.size() < too_large)
        ref += data.hello_world;
    ASSERT_THROW(arr.assign(ref.data(), ref.size()), std::length_error);
    // the content is unchanged
    ASSERT_EQ(arr.length(), 3u);
    for (size_t i = 0; i < arr.length(); ++i)
        ASSERT_EQ(arr[i], ch);

    // and again using an iterator
    ASSERT_THROW(arr.assign(ref.begin(), ref.end()), std::length_error);
    // the content is unchanged
    ASSERT_EQ(arr.length(), 3u);
    for (size_t i = 0; i < arr.length(); ++i)
        ASSERT_EQ(arr[i], ch);

    // using push_back()
    arr.clear();
    for (size_t i = 0; i < arr.max_size(); ++i)
        ASSERT_NO_THROW(arr.push_back(ch));
    ASSERT_THROW(arr.push_back(ch), std::length_error);
    // the content is unchanged
    ASSERT_EQ(arr.length(), arr.max_size());
    for (size_t i = 0; i < arr.length(); ++i)
        ASSERT_EQ(arr[i], ch);
}

/* truncating when exceeding max_size() during insert() */
TYPED_TEST(StorageArrayTest, InsertTruncationTests)
{
    using CharType = TypeParam; // gtest specific
    // this variant definitely won't throw
    using ArrayType = spsl::StorageArray<CharType, 64, spsl::policy::overflow::Truncate>;
    const TestData<CharType> data{};

    const CharType ch = data.hello_world[0];
    ArrayType arr;

    // insert too many characters
    arr.clear();
    arr.assign(arr.max_size() - 1, ch);
    ASSERT_NO_THROW(arr.insert(0, 100, ch));
    ASSERT_EQ(arr.length(), arr.max_size());
    ASSERT_EQ(arr.size(), arr.max_size());
    ASSERT_EQ(arr.capacity_left(), 0u);
    for (size_t i = 0; i < arr.length(); ++i)
        ASSERT_EQ(arr[i], ch);

    // insert a string that's too long
    arr.clear();
    const size_t n = 5;
    arr.assign(arr.max_size() - n, ch);
    ASSERT_NO_THROW(arr.insert(arr.length(), data.hello_world, data.hello_world_len));
    ASSERT_EQ(arr.length(), arr.max_size());
    ASSERT_EQ(arr.size(), arr.max_size());
    ASSERT_EQ(arr.capacity_left(), 0u);
    for (size_t i = 0; i < arr.length() - n; ++i)
        ASSERT_EQ(arr[i], ch);
    for (size_t i = 0; i < n; ++i)
        ASSERT_EQ(arr[arr.length() - n + i], data.hello_world[i]);

    // and again using an iterator
    arr.clear();
    arr.assign(arr.max_size() - n, ch);
    std::basic_string<CharType> ref(data.hello_world, data.hello_world_len);
    ASSERT_NO_THROW(arr.insert(arr.length(), ref.begin(), ref.end()));
    ASSERT_EQ(arr.length(), arr.max_size());
    ASSERT_EQ(arr.size(), arr.max_size());
    ASSERT_EQ(arr.capacity_left(), 0u);
    for (size_t i = 0; i < arr.length() - n; ++i)
        ASSERT_EQ(arr[i], ch);
    for (size_t i = 0; i < n; ++i)
        ASSERT_EQ(arr[arr.length() - n + i], data.hello_world[i]);
}

/* std::length_error when exceeding max_size() during insert() */
TYPED_TEST(StorageArrayTest, InsertLengthErrorTests)
{
    using CharType = TypeParam; // gtest specific
    // this variant definitely will throw
    using ArrayType = spsl::StorageArray<CharType, 64, spsl::policy::overflow::Throw>;
    const TestData<CharType> data{};

    ArrayType arr;
    const CharType ch = data.hello_world[0];
    arr.assign(3, ch);

    // insert too many characters
    ASSERT_THROW(arr.insert(0, 100, ch), std::length_error);
    // the content is unchanged
    ASSERT_EQ(arr.length(), 3u);
    for (size_t i = 0; i < arr.length(); ++i)
        ASSERT_EQ(arr[i], ch);

    // insert a string that's too long
    const std::basic_string<CharType> ref(arr.max_size(), ch);
    ASSERT_THROW(arr.insert(arr.length(), ref.data(), ref.size()), std::length_error);
    // the content is unchanged
    ASSERT_EQ(arr.length(), 3u);
    for (size_t i = 0; i < arr.length(); ++i)
        ASSERT_EQ(arr[i], ch);

    // and again using an iterator
    ASSERT_THROW(arr.insert(arr.length(), ref.begin(), ref.end()), std::length_error);
    // the content is unchanged
    ASSERT_EQ(arr.length(), 3u);
    for (size_t i = 0; i < arr.length(); ++i)
        ASSERT_EQ(arr[i], ch);
}

/* std::out_of_range when trying to insert past the end */
TYPED_TEST(StorageArrayTest, InsertRangeErrorTests)
{
    using CharType = TypeParam; // gtest specific
    using ArrayType = spsl::StorageArray<CharType, 64, spsl::policy::overflow::Truncate>;
    const TestData<CharType> data{};

    ArrayType arr;
    const CharType ch = data.hello_world[0];
    arr.assign(3, ch);

    // void insert(size_type index, size_type count, char_type ch)
    ASSERT_THROW(arr.insert(arr.size() + 1, 100, ch), std::out_of_range);
    // the content is unchanged
    ASSERT_EQ(arr.length(), 3u);
    for (size_t i = 0; i < arr.length(); ++i)
        ASSERT_EQ(arr[i], ch);

    // void insert(size_type index, const char_type* s, size_type n)
    ASSERT_THROW(arr.insert(arr.size() + 1, data.hello_world, data.hello_world_len),
                 std::out_of_range);
    // the content is unchanged
    ASSERT_EQ(arr.length(), 3u);
    for (size_t i = 0; i < arr.length(); ++i)
        ASSERT_EQ(arr[i], ch);

    // void insert(size_type index, InputIt first, InputIt last)
    const std::basic_string<CharType> ref(data.hello_world);
    ASSERT_THROW(arr.insert(arr.size() + 1, ref.begin(), ref.end()), std::out_of_range);
    // the content is unchanged
    ASSERT_EQ(arr.length(), 3u);
    for (size_t i = 0; i < arr.length(); ++i)
        ASSERT_EQ(arr[i], ch);
}


/* replace function */
TYPED_TEST(StorageArrayTest, ReplaceTests)
{
    using CharType = TypeParam; // gtest specific
    using ArrayType = spsl::StorageArray<CharType, 64>;
    using Traits = typename ArrayType::traits_type;
    const TestData<CharType> data{};
    using RefType = std::basic_string<CharType>;

    // replace(size_type pos, size_type count, const char_type* cstr, size_type count2)
    {
        ArrayType arr;
        RefType ref;

        // replace at the beginning
        arr.assign(data.hello_world, data.hello_world_len);
        arr.replace(0, 3, data.blablabla, data.blablabla_len);
        ref = data.blablabla;
        ref += data.hello_world + 3;
        ASSERT_EQ(arr.size(), ref.size());
        ASSERT_TRUE(Traits::compare(arr.data(), ref.data(), ref.size()) == 0);
        // with count = 0
        arr.assign(data.hello_world, data.hello_world_len);
        arr.replace(0, 0, data.blablabla, data.blablabla_len);
        ref = data.blablabla;
        ref += data.hello_world;
        ASSERT_EQ(arr.size(), ref.size());
        ASSERT_TRUE(Traits::compare(arr.data(), ref.data(), ref.size()) == 0);

        // replace in the middle
        arr.assign(data.hello_world, data.hello_world_len);
        arr.replace(3, 6, data.blablabla, data.blablabla_len);
        ref.assign(data.hello_world, 3);
        ref += data.blablabla;
        ref += data.hello_world + 9;
        ASSERT_EQ(arr.size(), ref.size());
        ASSERT_TRUE(Traits::compare(arr.data(), ref.data(), ref.size()) == 0);

        // replace at the end
        arr.assign(data.hello_world, data.hello_world_len);
        arr.replace(6, 6, data.blablabla, data.blablabla_len);
        ref.assign(data.hello_world, 6);
        ref += data.blablabla;
        ASSERT_EQ(arr.size(), ref.size());
        ASSERT_TRUE(Traits::compare(arr.data(), ref.data(), ref.size()) == 0);
    }

    // replace(size_type pos, size_type count, size_type count2, char_type ch)
    {
        ArrayType arr;
        RefType ref;
        const CharType b = data.blablabla[0];

        // replace at the beginning
        arr.assign(data.hello_world, data.hello_world_len);
        arr.replace(0, 3, 10, b);
        ref.assign(10, b);
        ref += data.hello_world + 3;
        ASSERT_EQ(arr.size(), ref.size());
        ASSERT_TRUE(Traits::compare(arr.data(), ref.data(), ref.size()) == 0);
        // with count = 0
        arr.assign(data.hello_world, data.hello_world_len);
        arr.replace(0, 0, 10, b);
        ref.assign(10, b);
        ref += data.hello_world;
        ASSERT_EQ(arr.size(), ref.size());
        ASSERT_TRUE(Traits::compare(arr.data(), ref.data(), ref.size()) == 0);

        // replace in the middle
        arr.assign(data.hello_world, data.hello_world_len);
        arr.replace(3, 6, 20, b);
        ref.assign(data.hello_world, 3);
        ref.append(20, b);
        ref += data.hello_world + 9;
        ASSERT_EQ(arr.size(), ref.size());
        ASSERT_TRUE(Traits::compare(arr.data(), ref.data(), ref.size()) == 0);

        // replace at the end
        arr.assign(data.hello_world, data.hello_world_len);
        arr.replace(6, 6, 13, b);
        ref.assign(data.hello_world, 6);
        ref.append(13, b);
        ASSERT_EQ(arr.size(), ref.size());
        ASSERT_TRUE(Traits::compare(arr.data(), ref.data(), ref.size()) == 0);
    }

    // replace(size_type pos, size_type count, InputIt first, InputIt last)
    {
        ArrayType arr;
        RefType ref;
        // we use a vector and it's iterators
        std::vector<CharType> vec;
        const CharType l = data.blablabla[1];
        vec.assign(12, l);

        // replace at the beginning
        arr.assign(data.hello_world, data.hello_world_len);
        arr.replace(0, 3, vec.begin(), vec.end());
        ref.assign(12, l);
        ref += data.hello_world + 3;
        ASSERT_EQ(arr.size(), ref.size());
        ASSERT_TRUE(Traits::compare(arr.data(), ref.data(), ref.size()) == 0);
        // with count = 0
        arr.assign(data.hello_world, data.hello_world_len);
        arr.replace(0, 0, vec.cbegin(), vec.cend());
        ref.assign(12, l);
        ref += data.hello_world;
        ASSERT_EQ(arr.size(), ref.size());
        ASSERT_TRUE(Traits::compare(arr.data(), ref.data(), ref.size()) == 0);

        // replace in the middle
        arr.assign(data.hello_world, data.hello_world_len);
        arr.replace(3, 6, vec.begin(), vec.begin() + 2);
        ref.assign(data.hello_world, 3);
        ref.append(2, l);
        ref += data.hello_world + 9;
        ASSERT_EQ(arr.size(), ref.size());
        ASSERT_TRUE(Traits::compare(arr.data(), ref.data(), ref.size()) == 0);

        // replace at the end
        arr.assign(data.hello_world, data.hello_world_len);
        arr.replace(6, 6, vec.rbegin(), vec.rend());
        ref.assign(data.hello_world, 6);
        ref.append(12, l);
        ASSERT_EQ(arr.size(), ref.size());
        ASSERT_TRUE(Traits::compare(arr.data(), ref.data(), ref.size()) == 0);
    }
}

/* truncating when exceeding max_size() during replace() */
TYPED_TEST(StorageArrayTest, ReplaceTruncationTests)
{
    using CharType = TypeParam; // gtest specific
    // this variant definitely won't throw
    using ArrayType = spsl::StorageArray<CharType, 64, spsl::policy::overflow::Truncate>;
    using Traits = typename ArrayType::traits_type;
    const TestData<CharType> data{};

    const CharType ch = data.hello_world[0];
    const CharType ch2 = data.hello_world[1];
    ArrayType arr;

    arr.assign(arr.max_size() - 3, ch);
    ASSERT_EQ(arr.size(), arr.max_size() - 3);
    // 3 more characters can fit
    ASSERT_NO_THROW(arr.replace(0, 0, 3, ch));
    ASSERT_EQ(arr.size(), arr.max_size());

    // any character more and it will be truncated
    // (1) replace with individual characters
    ASSERT_NO_THROW(arr.replace(0, 1, 12, ch2));
    ASSERT_EQ(arr.size(), arr.max_size());
    for (size_t i = 0; i < 12; ++i)
        ASSERT_EQ(arr[i], ch2);
    for (size_t i = 12; i < arr.size(); ++i)
        ASSERT_EQ(arr[i], ch);

    // (2) replace with a string
    ASSERT_NO_THROW(arr.replace(0, 0, data.blablabla, data.blablabla_len));
    ASSERT_TRUE(Traits::compare(arr.data(), data.blablabla, data.blablabla_len) == 0);
    for (size_t i = data.blablabla_len; i < data.blablabla_len + 12; ++i)
        ASSERT_EQ(arr[i], ch2);
    for (size_t i = data.blablabla_len + 12; i < arr.size(); ++i)
        ASSERT_EQ(arr[i], ch);

    // (3) replace with an iterator range
    const std::basic_string<CharType> s(3, ch);
    ASSERT_NO_THROW(arr.replace(0, 1, s.begin(), s.end()));
    for (size_t i = 0; i < 3; ++i)
        ASSERT_EQ(arr[i], ch);
    ASSERT_TRUE(Traits::compare(arr.data() + 3, data.blablabla + 1, data.blablabla_len - 1) == 0);
    for (size_t i = data.blablabla_len + 2; i < data.blablabla_len + 14; ++i)
        ASSERT_EQ(arr[i], ch2);
    for (size_t i = data.blablabla_len + 14; i < arr.size(); ++i)
        ASSERT_EQ(arr[i], ch);
}

/* std::length_error when exceeding max_size() during replace() */
TYPED_TEST(StorageArrayTest, ReplaceLengthErrorTests)
{
    using CharType = TypeParam; // gtest specific
    // this variant definitely will throw
    using ArrayType = spsl::StorageArray<CharType, 64, spsl::policy::overflow::Throw>;
    using Traits = typename ArrayType::traits_type;
    const TestData<CharType> data{};

    const CharType ch = data.hello_world[0];
    const CharType ch2 = data.hello_world[1];
    ArrayType arr;

    arr.assign(arr.max_size() - 3, ch);
    ASSERT_EQ(arr.size(), arr.max_size() - 3);
    // 3 more characters can fit
    ASSERT_NO_THROW(arr.replace(0, 0, 3, ch));
    ASSERT_EQ(arr.size(), arr.max_size());

    // any character more and std::length_error is thrown, but the string is unchanged
    const std::basic_string<CharType> ref(arr.data(), arr.size());
    // (1) replace with individual characters
    ASSERT_THROW(arr.replace(0, 1, 12, ch2), std::length_error);
    ASSERT_EQ(arr.size(), ref.size());
    ASSERT_TRUE(Traits::compare(arr.data(), ref.data(), ref.size()) == 0);

    // (2) replace with a string
    ASSERT_THROW(arr.replace(0, data.blablabla_len - 1, data.blablabla, data.blablabla_len),
                 std::length_error);
    ASSERT_EQ(arr.size(), ref.size());
    ASSERT_TRUE(Traits::compare(arr.data(), ref.data(), ref.size()) == 0);

    // (3) replace with an iterator range
    const std::basic_string<CharType> s(3, ch);
    ASSERT_THROW(arr.replace(0, 1, s.begin(), s.end()), std::length_error);
    ASSERT_EQ(arr.size(), ref.size());
    ASSERT_TRUE(Traits::compare(arr.data(), ref.data(), ref.size()) == 0);
}

/* erase function */
TYPED_TEST(StorageArrayTest, EraseTests)
{
    using CharType = TypeParam; // gtest specific
    using ArrayType = spsl::StorageArray<CharType, 64>;
    using Traits = typename ArrayType::traits_type;
    const TestData<CharType> data{};
    using RefType = std::basic_string<CharType>;

    // void erase(size_type index, size_type count)
    ArrayType arr;
    RefType ref;
    arr.assign(data.hello_world, data.hello_world_len);
    ref = data.hello_world;
    ASSERT_EQ(arr.size(), ref.size());
    ASSERT_TRUE(Traits::compare(arr.data(), ref.data(), ref.size()) == 0);

    arr.erase(0, 1);
    ref.erase(0, 1);
    ASSERT_EQ(arr.size(), ref.size());
    ASSERT_TRUE(Traits::compare(arr.data(), ref.data(), ref.size()) == 0);

    arr.erase(5, 3);
    ref.erase(5, 3);
    ASSERT_EQ(arr.size(), ref.size());
    ASSERT_TRUE(Traits::compare(arr.data(), ref.data(), ref.size()) == 0);

    arr.erase(0, arr.size());
    ref.erase(0, ref.size());
    ASSERT_EQ(arr.size(), ref.size());
    ASSERT_TRUE(Traits::compare(arr.data(), ref.data(), ref.size()) == 0);
    ASSERT_TRUE(arr.empty());
}
