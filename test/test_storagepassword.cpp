/**
 * @file	Special Purpose Strings Library: test_storagepassword.cpp
 * @author	Daniel Evers
 * @brief   StoragePassword unit tests
 * @license MIT
 */

#include <gtest/gtest.h>

#include "spsl/storage_password.hpp"
#include "testdata.hpp"

/**
 * Custom allocator that checks whether a memory area was zero'd out before freeing it.
 */
template <typename T>
struct WipeCheckAllocator
{
    using pointer = T*;
    using size_type = std::size_t;

    pointer allocate(size_type n, const void* = nullptr)
    {
        if (n > this->max_size())
            throw std::bad_alloc();

        return static_cast<pointer>(malloc(n * sizeof(T)));
    }

    void deallocate(pointer ptr, size_type size)
    {
        for (size_t i = 0; i < size; ++i)
        {
            ASSERT_EQ(ptr[i], static_cast<T>(0));
        }
        free(ptr);
    }

    size_type max_size() const noexcept { return size_t(-1) / sizeof(T); }
};

template <typename CharType>
class StoragePasswordTest : public ::testing::Test
{
};

// all character types we want to test
using CharTypes = testing::Types<char, wchar_t>;


TYPED_TEST_CASE(StoragePasswordTest, CharTypes);

/* constructor tests */
TYPED_TEST(StoragePasswordTest, ConstructorTests)
{
    using CharType = TypeParam; // gtest specific
    using StorageType = spsl::StoragePassword<CharType>;
    const CharType nul = StorageType::nul;

    const StorageType s1;
    ASSERT_EQ(s1.capacity(), 0);
    ASSERT_EQ(s1.max_size(), static_cast<size_t>(-1) / sizeof(CharType));

    ASSERT_TRUE(s1.empty());
    ASSERT_EQ(s1.length(), 0);
    ASSERT_EQ(s1.length(), s1.size());
    // valid content?
    ASSERT_NE(s1.data(), nullptr);
    ASSERT_EQ(s1.data()[0], nul);
    ASSERT_EQ(s1[0], nul);

    // copy & move constructor are available
    StorageType s2(s1);
    StorageType s3(std::move(s2));
}

/* assignment functions */
TYPED_TEST(StoragePasswordTest, AssignmentTests)
{
    using CharType = TypeParam; // gtest specific
    using StorageType = spsl::StoragePassword<CharType>;
    const TestData<CharType> data{};
    using Traits = typename StorageType::traits_type;

    StorageType s;
    // assign a string
    s.assign(data.hello_world, data.hello_world_len);
    ASSERT_EQ(s.length(), Traits::length(s.data()));
    ASSERT_EQ(s.length(), data.hello_world_len);
    ASSERT_TRUE(Traits::compare(s.data(), data.hello_world, data.hello_world_len) == 0);

    // assign something else
    s.assign(data.blablabla, data.blablabla_len);
    ASSERT_EQ(s.length(), Traits::length(s.data()));
    ASSERT_EQ(s.length(), data.blablabla_len);
    ASSERT_TRUE(Traits::compare(s.data(), data.blablabla, data.blablabla_len) == 0);

    // assign a repeated character
    const CharType ch = data.hello_world[0];
    s.assign(33, ch);
    ASSERT_EQ(s.length(), 33);
    for (size_t i = 0; i < 33; ++i)
    {
        ASSERT_EQ(s[i], ch);
    }

    // assign an iterator range - we use std::basic_string here
    const std::basic_string<CharType> bs(data.hello_world);
    s.assign(bs.begin(), bs.end());
    ASSERT_EQ(s.length(), bs.length());
    ASSERT_TRUE(Traits::compare(s.data(), data.hello_world, data.hello_world_len) == 0);
    ASSERT_TRUE(bs == s.data());
}

/* push_back/pop_back functions */
TYPED_TEST(StoragePasswordTest, PushPopTests)
{
    using CharType = TypeParam; // gtest specific
    // use an allocation block size of '4' to force reallocations
    using StorageType = spsl::StoragePassword<CharType, 4>;
    const TestData<CharType> data{};
    using Traits = typename StorageType::traits_type;

    StorageType s;
    // append a string byte for byte
    for (size_t i = 0; i < data.hello_world_len; ++i)
    {
        ASSERT_EQ(s.length(), i);
        s.push_back(data.hello_world[i]);
        ASSERT_EQ(s.length(), i + 1);
    }
    ASSERT_EQ(s.length(), data.hello_world_len);
    ASSERT_TRUE(Traits::compare(s.data(), data.hello_world, data.hello_world_len) == 0);

    // remove the last byte
    for (size_t i = 0; i < data.hello_world_len; ++i)
    {
        ASSERT_EQ(s.length(), data.hello_world_len - i);
        s.pop_back();
        ASSERT_EQ(s.length(), data.hello_world_len - i - 1);
    }
    ASSERT_EQ(s.length(), 0);
    ASSERT_TRUE(s.empty());
}

/* insert functions */
TYPED_TEST(StoragePasswordTest, InsertTests)
{
    using CharType = TypeParam; // gtest specific
    using StorageType = spsl::StoragePassword<CharType>;
    const TestData<CharType> data{};
    using Traits = typename StorageType::traits_type;

    // prototype: void insert(size_type index, size_type count, char_type ch)
    StorageType s;
    const CharType ch = data.hello_world[0];
    s.insert(0, 5, ch);
    ASSERT_EQ(s.length(), 5);
    for (size_t i = 0; i < 5; ++i)
        ASSERT_EQ(s[i], ch);

    // insert again at the beginning
    const CharType ch2 = data.hello_world[1];
    s.insert(0, 5, ch2);
    ASSERT_EQ(s.length(), 10);
    for (size_t i = 0; i < 5; ++i)
        ASSERT_EQ(s[i], ch2);
    for (size_t i = 5; i < 10; ++i)
        ASSERT_EQ(s[i], ch);

    // insert in the middle
    const CharType ch3 = data.hello_world[2];
    s.insert(5, 10, ch3);
    ASSERT_EQ(s.length(), 20);
    for (size_t i = 0; i < 5; ++i)
        ASSERT_EQ(s[i], ch2);
    for (size_t i = 5; i < 15; ++i)
        ASSERT_EQ(s[i], ch3);
    for (size_t i = 15; i < 20; ++i)
        ASSERT_EQ(s[i], ch);

    const std::basic_string<CharType> bs(data.blablabla);

    // insert a string
    StorageType str2(s);
    s.insert(15, bs.data(), bs.size());
    ASSERT_EQ(s.length(), 20 + bs.length());

    for (size_t i = 0; i < 5; ++i)
        ASSERT_EQ(s[i], ch2);
    for (size_t i = 5; i < 15; ++i)
        ASSERT_EQ(s[i], ch3);
    ASSERT_TRUE(Traits::compare(s.data() + 15, data.blablabla, data.blablabla_len) == 0);
    for (size_t i = 15 + data.blablabla_len; i < 20 + data.blablabla_len; ++i)
        ASSERT_EQ(s[i], ch);

    // again using an iterator range
    s = str2;
    s.insert(15, bs.begin(), bs.end());
    ASSERT_EQ(s.length(), 20 + bs.length());

    for (size_t i = 0; i < 5; ++i)
        ASSERT_EQ(s[i], ch2);
    for (size_t i = 5; i < 15; ++i)
        ASSERT_EQ(s[i], ch3);
    ASSERT_TRUE(Traits::compare(s.data() + 15, data.blablabla, data.blablabla_len) == 0);
    for (size_t i = 15 + data.blablabla_len; i < 20 + data.blablabla_len; ++i)
        ASSERT_EQ(s[i], ch);
}

/* std::out_of_range when trying to insert past the end */
TYPED_TEST(StoragePasswordTest, InsertRangeErrorTests)
{
    using CharType = TypeParam; // gtest specific
    using StorageType = spsl::StoragePassword<CharType>;
    const TestData<CharType> data{};

    StorageType s;
    const CharType ch = data.hello_world[0];
    s.assign(3, ch);

    // void insert(size_type index, size_type count, char_type ch)
    ASSERT_THROW(s.insert(s.size() + 1, 100, ch), std::out_of_range);
    // the content is unchanged
    ASSERT_EQ(s.length(), 3);
    for (size_t i = 0; i < s.length(); ++i)
        ASSERT_EQ(s[i], ch);

    // void insert(size_type index, const char_type* s, size_type n)
    ASSERT_THROW(s.insert(s.size() + 1, data.hello_world, data.hello_world_len), std::out_of_range);
    // the content is unchanged
    ASSERT_EQ(s.length(), 3);
    for (size_t i = 0; i < s.length(); ++i)
        ASSERT_EQ(s[i], ch);

    // void insert(size_type index, InputIt first, InputIt last)
    const std::basic_string<CharType> ref(data.hello_world);
    ASSERT_THROW(s.insert(s.size() + 1, ref.begin(), ref.end()), std::out_of_range);
    // the content is unchanged
    ASSERT_EQ(s.length(), 3);
    for (size_t i = 0; i < s.length(); ++i)
        ASSERT_EQ(s[i], ch);
}

/* append functions */
TYPED_TEST(StoragePasswordTest, AppendTests)
{
    using CharType = TypeParam; // gtest specific
    using StorageType = spsl::StoragePassword<CharType>;
    const TestData<CharType> data{};

    // apply all functions to the array and std::basic_string at the same time and make sure they
    // stay identical
    StorageType s;
    std::basic_string<CharType> ref;

    // void append(const char_type* s, size_type n)
    s.append(data.hello_world, data.hello_world_len);
    ref.append(data.hello_world, data.hello_world_len);
    ASSERT_EQ(ref, s.data());
    ASSERT_EQ(ref.length(), s.length());
    ASSERT_EQ(ref.size(), s.size());

    s.append(data.blablabla, data.blablabla_len);
    ref.append(data.blablabla, data.blablabla_len);
    ASSERT_EQ(ref, s.data());
    ASSERT_EQ(ref.length(), s.length());
    ASSERT_EQ(ref.size(), s.size());

    // void append(size_type count, char_type ch)
    const CharType ch = data.hello_world[0];
    s.append(20, ch);
    ref.append(20, ch);
    ASSERT_EQ(ref, s.data());
    ASSERT_EQ(ref.length(), s.length());
    ASSERT_EQ(ref.size(), s.size());

    // void append(InputIt first, InputIt last)
    const std::basic_string<CharType> bs(data.blablabla);
    s.append(bs.begin(), bs.end());
    ref.append(bs.begin(), bs.end());
    ASSERT_EQ(ref, s.data());
    ASSERT_EQ(ref.length(), s.length());
    ASSERT_EQ(ref.size(), s.size());
}

/* swap function */
TYPED_TEST(StoragePasswordTest, SwapTests)
{
    using CharType = TypeParam; // gtest specific
    using StorageType = spsl::StoragePassword<CharType>;
    const TestData<CharType> data{};
    using Traits = typename StorageType::traits_type;

    StorageType str1, str2;
    // let's start with swapping empty strings...
    ASSERT_TRUE(str1.empty());
    ASSERT_TRUE(str2.empty());
    str1.swap(str2);
    ASSERT_TRUE(str1.empty());
    ASSERT_TRUE(str2.empty());

    // assign + swap
    str1.assign(data.hello_world, data.hello_world_len);
    ASSERT_EQ(str1.length(), data.hello_world_len);
    ASSERT_TRUE(Traits::compare(str1.data(), data.hello_world, data.hello_world_len) == 0);
    ASSERT_EQ(str2.length(), 0);
    str1.swap(str2);
    ASSERT_EQ(str2.length(), data.hello_world_len);
    ASSERT_TRUE(Traits::compare(str2.data(), data.hello_world, data.hello_world_len) == 0);
    ASSERT_EQ(str1.length(), 0);

    // swap back
    str1.swap(str2);
    ASSERT_EQ(str1.length(), data.hello_world_len);
    ASSERT_TRUE(Traits::compare(str1.data(), data.hello_world, data.hello_world_len) == 0);
    ASSERT_EQ(str2.length(), 0);

    // now swap 2 non-empty strings
    str1.assign(data.hello_world, data.hello_world_len);
    ASSERT_EQ(str1.length(), data.hello_world_len);
    ASSERT_TRUE(Traits::compare(str1.data(), data.hello_world, data.hello_world_len) == 0);
    str2.assign(data.blablabla, data.blablabla_len);
    ASSERT_EQ(str2.length(), data.blablabla_len);
    ASSERT_TRUE(Traits::compare(str2.data(), data.blablabla, data.blablabla_len) == 0);
    str2.swap(str1);
    ASSERT_EQ(str2.length(), data.hello_world_len);
    ASSERT_TRUE(Traits::compare(str2.data(), data.hello_world, data.hello_world_len) == 0);
    ASSERT_EQ(str1.length(), data.blablabla_len);
    ASSERT_TRUE(Traits::compare(str1.data(), data.blablabla, data.blablabla_len) == 0);
}

/* resize function */
TYPED_TEST(StoragePasswordTest, ResizeTests)
{
    using CharType = TypeParam; // gtest specific
    // block size 32
    using StorageType = spsl::StoragePassword<CharType, 32>;
    const TestData<CharType> data{};
    using size_type = typename StorageType::size_type;
    const CharType nul = StorageType::nul;

    StorageType s;
    // empty
    ASSERT_EQ(s.size(), 0);
    ASSERT_EQ(s.length(), 0);
    const CharType ch = data.hello_world[0];

    // resize + 10 characters
    s.resize(10, ch);
    ASSERT_EQ(s.size(), 10);
    ASSERT_EQ(s.length(), 10);
    for (size_t i = 0; i < 10; ++i)
        ASSERT_EQ(s[i], ch);
    ASSERT_EQ(s[10], nul);

    // again + 10 characters
    s.resize(20, ch);
    ASSERT_EQ(s.size(), 20);
    ASSERT_EQ(s.length(), 20);
    for (size_t i = 0; i < 20; ++i)
        ASSERT_EQ(s[i], ch);
    ASSERT_EQ(s[20], nul);

    // shrink 3 characters
    s.resize(17, ch);
    ASSERT_EQ(s.size(), 17);
    ASSERT_EQ(s.length(), 17);
    for (size_t i = 0; i < 17; ++i)
        ASSERT_EQ(s[i], ch);
    ASSERT_EQ(s[17], nul);

    // shrink_to_fit may reduce the capacity, but not the size
    const size_type oldCapa = s.capacity();
    const size_type oldSize = s.size();
    s.shrink_to_fit();
    ASSERT_LE(oldCapa, s.capacity());
    ASSERT_EQ(oldSize, s.size());
}

/* allocation function */
TYPED_TEST(StoragePasswordTest, ReallocTests)
{
    using CharType = TypeParam; // gtest specific
    // block size 32
    using StorageType = spsl::StoragePassword<CharType, 32>;
    using size_type = typename StorageType::size_type;
    const size_type block_size = StorageType::block_size();
    const TestData<CharType> data{};
    const CharType nul = StorageType::nul;
    const CharType ch(data.hello_world[2]);

    StorageType s;
    ASSERT_EQ(s.size(), 0);
    ASSERT_EQ(s.capacity(), 0);

    // requesting < block_size allocates a full block
    s.reserve(block_size - 1);
    ASSERT_EQ(s.size(), 0);
    ASSERT_EQ(s.capacity(), block_size);

    // now request more than 1 block
    s.reserve(block_size + 1);
    ASSERT_EQ(s.size(), 0);
    ASSERT_EQ(s.capacity(), 2 * block_size);

    // the capacity won't change if we assign a string that fits
    s.resize(block_size / 2, ch);
    ASSERT_EQ(s.size(), block_size / 2);
    ASSERT_EQ(s.capacity(), 2 * block_size);
    for (size_type i = 0; i < s.size(); ++i)
        ASSERT_EQ(s[i], ch);
    ASSERT_EQ(s[s.size()], nul);

    // shrink_to_fit now reduces the size to 1 block
    s.shrink_to_fit();
    ASSERT_EQ(s.size(), block_size / 2);
    ASSERT_EQ(s.capacity(), block_size);
    for (size_type i = 0; i < s.size(); ++i)
        ASSERT_EQ(s[i], ch);
    ASSERT_EQ(s[s.size()], nul);

    // clear + shrink frees all memory
    s.clear();
    s.shrink_to_fit();
    ASSERT_EQ(s.size(), 0);
    ASSERT_EQ(s.capacity(), 0);
}

/* wiping memory */
TYPED_TEST(StoragePasswordTest, WipeTests)
{
    // verify that the buffer is wiped automatically upon destruction
    // -> we do this by providing a custom allocator that performs these checks

    using CharType = TypeParam; // gtest specific
    using StorageType = spsl::StoragePassword<CharType, 32, WipeCheckAllocator<CharType>>;
    using size_type = typename StorageType::size_type;
    const TestData<CharType> data{};
    const CharType ch = data.blablabla[0];
    const CharType nul = StorageType::nul;

    StorageType s;
    s.assign(data.hello_world, data.hello_world_len);
    ASSERT_TRUE(!s.empty());

    // removing the last character will wipe it's storage
    size_type index = s.size() - 1;
    ASSERT_NE(s[index], nul);
    s.pop_back();
    ASSERT_EQ(s[index], nul);

    // resize to shrink, then make sure the rest was wiped
    s.resize(index / 2, ch);
    ASSERT_EQ(s.size(), index / 2);
    for (size_type i = s.size(); i < index; ++i)
        ASSERT_EQ(s[i], nul);

    // great, now clear the whole string
    s.clear();
    ASSERT_GT(s.capacity(), 0u);
    for (size_type i = 0; i < s.capacity(); ++i)
        ASSERT_EQ(s[i], nul);

    // verify that the buffer is wiped automatically upon destruction
    {
        StorageType s2;
        for (size_t i = 0; i < 100; ++i)
            s2.append(data.blablabla, data.blablabla_len);
    }
}

/* replace functions */
TYPED_TEST(StoragePasswordTest, ReplaceTests)
{
    using CharType = TypeParam; // gtest specific
    using StorageType = spsl::StoragePassword<CharType, 32>;
    using Traits = typename StorageType::traits_type;
    const TestData<CharType> data{};
    using RefType = std::basic_string<CharType>;

    // replace(size_type pos, size_type count, const char_type* cstr, size_type count2)
    {
        StorageType s;
        RefType ref;

        // replace at the beginning
        s.assign(data.hello_world, data.hello_world_len);
        s.replace(0, 3, data.blablabla, data.blablabla_len);
        ref = data.blablabla;
        ref += data.hello_world + 3;
        ASSERT_EQ(s.size(), ref.size());
        ASSERT_TRUE(Traits::compare(s.data(), ref.data(), ref.size()) == 0);
        // with count = 0
        s.assign(data.hello_world, data.hello_world_len);
        s.replace(0, 0, data.blablabla, data.blablabla_len);
        ref = data.blablabla;
        ref += data.hello_world;
        ASSERT_EQ(s.size(), ref.size());
        ASSERT_TRUE(Traits::compare(s.data(), ref.data(), ref.size()) == 0);

        // replace in the middle
        s.assign(data.hello_world, data.hello_world_len);
        s.replace(3, 6, data.blablabla, data.blablabla_len);
        ref.assign(data.hello_world, 3);
        ref += data.blablabla;
        ref += data.hello_world + 9;
        ASSERT_EQ(s.size(), ref.size());
        ASSERT_TRUE(Traits::compare(s.data(), ref.data(), ref.size()) == 0);

        // replace at the end
        s.assign(data.hello_world, data.hello_world_len);
        s.replace(6, 6, data.blablabla, data.blablabla_len);
        ref.assign(data.hello_world, 6);
        ref += data.blablabla;
        ASSERT_EQ(s.size(), ref.size());
        ASSERT_TRUE(Traits::compare(s.data(), ref.data(), ref.size()) == 0);
    }

    // replace(size_type pos, size_type count, size_type count2, char_type ch)
    {
        StorageType s;
        RefType ref;
        const CharType b = data.blablabla[0];

        // replace at the beginning
        s.assign(data.hello_world, data.hello_world_len);
        s.replace(0, 3, 10, b);
        ref.assign(10, b);
        ref += data.hello_world + 3;
        ASSERT_EQ(s.size(), ref.size());
        ASSERT_TRUE(Traits::compare(s.data(), ref.data(), ref.size()) == 0);
        // with count = 0
        s.assign(data.hello_world, data.hello_world_len);
        s.replace(0, 0, 10, b);
        ref.assign(10, b);
        ref += data.hello_world;
        ASSERT_EQ(s.size(), ref.size());
        ASSERT_TRUE(Traits::compare(s.data(), ref.data(), ref.size()) == 0);

        // replace in the middle
        s.assign(data.hello_world, data.hello_world_len);
        s.replace(3, 6, 20, b);
        ref.assign(data.hello_world, 3);
        ref.append(20, b);
        ref += data.hello_world + 9;
        ASSERT_EQ(s.size(), ref.size());
        ASSERT_TRUE(Traits::compare(s.data(), ref.data(), ref.size()) == 0);

        // replace at the end
        s.assign(data.hello_world, data.hello_world_len);
        s.replace(6, 6, 13, b);
        ref.assign(data.hello_world, 6);
        ref.append(13, b);
        ASSERT_EQ(s.size(), ref.size());
        ASSERT_TRUE(Traits::compare(s.data(), ref.data(), ref.size()) == 0);
    }

    // replace(size_type pos, size_type count, InputIt first, InputIt last)
    {
        StorageType s;
        RefType ref;
        // we use a vector and it's iterators
        std::vector<CharType> vec;
        const CharType l = data.blablabla[1];
        vec.assign(12, l);

        // replace at the beginning
        s.assign(data.hello_world, data.hello_world_len);
        s.replace(0, 3, vec.begin(), vec.end());
        ref.assign(12, l);
        ref += data.hello_world + 3;
        ASSERT_EQ(s.size(), ref.size());
        ASSERT_TRUE(Traits::compare(s.data(), ref.data(), ref.size()) == 0);
        // with count = 0
        s.assign(data.hello_world, data.hello_world_len);
        s.replace(0, 0, vec.cbegin(), vec.cend());
        ref.assign(12, l);
        ref += data.hello_world;
        ASSERT_EQ(s.size(), ref.size());
        ASSERT_TRUE(Traits::compare(s.data(), ref.data(), ref.size()) == 0);

        // replace in the middle
        s.assign(data.hello_world, data.hello_world_len);
        s.replace(3, 6, vec.begin(), vec.begin() + 2);
        ref.assign(data.hello_world, 3);
        ref.append(2, l);
        ref += data.hello_world + 9;
        ASSERT_EQ(s.size(), ref.size());
        ASSERT_TRUE(Traits::compare(s.data(), ref.data(), ref.size()) == 0);

        // replace at the end
        s.assign(data.hello_world, data.hello_world_len);
        s.replace(6, 6, vec.rbegin(), vec.rend());
        ref.assign(data.hello_world, 6);
        ref.append(12, l);
        ASSERT_EQ(s.size(), ref.size());
        ASSERT_TRUE(Traits::compare(s.data(), ref.data(), ref.size()) == 0);
    }
}

/* copy & move tests */
TYPED_TEST(StoragePasswordTest, CopyAndMoveTests)
{
    using CharType = TypeParam; // gtest specific
    using StorageType = spsl::StoragePassword<CharType>;
    const TestData<CharType> data{};

    // create 2 distinct page allocator instances
    spsl::SensitivePageAllocator alloc1, alloc2;
    // use them in 2 different strings
    StorageType string1{ spsl::SensitiveSegmentAllocator<CharType>(alloc1) };
    string1.assign(data.hello_world, data.hello_world_len);
    StorageType string2{ spsl::SensitiveSegmentAllocator<CharType>(alloc2) };
    string2.assign(data.hello_world, data.hello_world_len);

    // same content, different allocators
    ASSERT_STREQ(string1.data(), string2.data());
    ASSERT_EQ(string1.getAllocator().pageAllocator(), &alloc1);
    ASSERT_EQ(string2.getAllocator().pageAllocator(), &alloc2);

    // swap: allocators are swapped, too
    std::swap(string1, string2);
    ASSERT_STREQ(string1.data(), string2.data());
    ASSERT_EQ(string1.getAllocator().pageAllocator(), &alloc2);
    ASSERT_EQ(string2.getAllocator().pageAllocator(), &alloc1);

    // change the content and swap back
    string1.assign(data.blablabla, data.blablabla_len);
    ASSERT_STREQ(string1.data(), data.blablabla);
    ASSERT_STREQ(string2.data(), data.hello_world);
    ASSERT_STRNE(string1.data(), string2.data());
    std::swap(string1, string2);
    ASSERT_STREQ(string1.data(), data.hello_world);
    ASSERT_STREQ(string2.data(), data.blablabla);
    ASSERT_EQ(string1.getAllocator().pageAllocator(), &alloc1);
    ASSERT_EQ(string2.getAllocator().pageAllocator(), &alloc2);

    // using the copy or move constructor copies the allocator
    StorageType string3(string1);
    StorageType string4(std::move(string1));
    ASSERT_EQ(string1.getAllocator().pageAllocator(), &alloc1);
    ASSERT_EQ(string3.getAllocator().pageAllocator(), &alloc1);
    ASSERT_EQ(string4.getAllocator().pageAllocator(), &alloc1);
    // copy assignment doesn't
    string3 = string2;
    ASSERT_EQ(string3.getAllocator().pageAllocator(), &alloc1);

    // moving swaps the allocator
    auto defaultAlloc = &spsl::SensitivePageAllocator::getDefaultInstance();
    StorageType string5;
    ASSERT_EQ(string5.getAllocator().pageAllocator(), defaultAlloc);
    string5 = std::move(string1);
    ASSERT_EQ(string5.getAllocator().pageAllocator(), &alloc1);
    ASSERT_EQ(string1.getAllocator().pageAllocator(), defaultAlloc);
    string5.swap(string1);
    ASSERT_EQ(string1.getAllocator().pageAllocator(), &alloc1);
    ASSERT_EQ(string5.getAllocator().pageAllocator(), defaultAlloc);
}

/* erase function */
TYPED_TEST(StoragePasswordTest, EraseTests)
{
    using CharType = TypeParam; // gtest specific
    using StorageType = spsl::StoragePassword<CharType>;
    using Traits = typename StorageType::traits_type;
    const TestData<CharType> data{};
    using RefType = std::basic_string<CharType>;

    // void erase(size_type index, size_type count)
    StorageType s;
    RefType ref;
    s.assign(data.hello_world, data.hello_world_len);
    ref = data.hello_world;
    ASSERT_EQ(s.size(), ref.size());
    ASSERT_TRUE(Traits::compare(s.data(), ref.data(), ref.size()) == 0);

    s.erase(0, 1);
    ref.erase(0, 1);
    ASSERT_EQ(s.size(), ref.size());
    ASSERT_TRUE(Traits::compare(s.data(), ref.data(), ref.size()) == 0);

    s.erase(5, 3);
    ref.erase(5, 3);
    ASSERT_EQ(s.size(), ref.size());
    ASSERT_TRUE(Traits::compare(s.data(), ref.data(), ref.size()) == 0);

    s.erase(0, s.size());
    ref.erase(0, ref.size());
    ASSERT_EQ(s.size(), ref.size());
    ASSERT_TRUE(Traits::compare(s.data(), ref.data(), ref.size()) == 0);
    ASSERT_TRUE(s.empty());
}
