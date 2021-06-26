/**
 * @author	Daniel Evers
 * @brief   StoragePassword unit tests
 * @license MIT
 */

#include "catch.hpp"

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
        constexpr T nul{};
        for (size_t i = 0; i < size; ++i)
        {
            REQUIRE(ptr[i] == nul);
        }
        free(ptr);
    }

    size_type max_size() const noexcept { return size_t(-1) / sizeof(T); }
};


// all character types we want to test
using CharTypes = std::tuple<char, wchar_t>;


// generic string comparisons
static int compareStrings(const char* s1, const char* s2)
{
    return strcmp(s1, s2);
}
static int compareStrings(const wchar_t* s1, const wchar_t* s2)
{
    return wcscmp(s1, s2);
}

/* constructor tests */
TEMPLATE_LIST_TEST_CASE("StoragePassword constructor", "[storage_password]", CharTypes)
{
    using CharType = TestType;
    using StorageType = spsl::StoragePassword<CharType>;
    const CharType nul = StorageType::nul;

    const StorageType s1;
    REQUIRE(s1.capacity() == 0u);
    REQUIRE(s1.max_size() == static_cast<size_t>(-1) / sizeof(CharType));

    REQUIRE(s1.empty());
    REQUIRE(s1.length() == 0u);
    REQUIRE(s1.length() == s1.size());
    // valid content?
    REQUIRE(s1.data() != nullptr);
    REQUIRE(s1.data()[0] == nul);
    REQUIRE(s1[0] == nul);

    // copy & move constructor are available
    StorageType s2(s1);
    StorageType s3(std::move(s2));
}

/* assignment functions */
TEMPLATE_LIST_TEST_CASE("StoragePassword assignment", "[storage_password]", CharTypes)
{
    using CharType = TestType;
    using StorageType = spsl::StoragePassword<CharType>;
    const TestData<CharType> data;
    using Traits = typename StorageType::traits_type;

    StorageType s;
    // assign a string
    s.assign(data.hello_world, data.hello_world_len);
    REQUIRE(s.length() == Traits::length(s.data()));
    REQUIRE(s.length() == data.hello_world_len);
    REQUIRE(Traits::compare(s.data(), data.hello_world, data.hello_world_len) == 0);

    // assign something else
    s.assign(data.blablabla, data.blablabla_len);
    REQUIRE(s.length() == Traits::length(s.data()));
    REQUIRE(s.length() == data.blablabla_len);
    REQUIRE(Traits::compare(s.data(), data.blablabla, data.blablabla_len) == 0);

    // assign a repeated character
    const CharType ch = data.hello_world[0];
    s.assign(33, ch);
    REQUIRE(s.length() == 33u);
    for (size_t i = 0; i < 33; ++i)
    {
        REQUIRE(s[i] == ch);
    }

    // assign an iterator range - we use std::basic_string here
    const std::basic_string<CharType> bs(data.hello_world);
    s.assign(bs.begin(), bs.end());
    REQUIRE(s.length() == bs.length());
    REQUIRE(Traits::compare(s.data(), data.hello_world, data.hello_world_len) == 0);
    REQUIRE(bs == s.data());
}

/* push_back/pop_back functions */
TEMPLATE_LIST_TEST_CASE("StoragePassword push and pop", "[storage_password]", CharTypes)
{
    using CharType = TestType;
    // use an allocation block size of '4' to force reallocations
    using StorageType = spsl::StoragePassword<CharType, 4>;
    const TestData<CharType> data;
    using Traits = typename StorageType::traits_type;

    StorageType s;
    // append a string byte for byte
    for (size_t i = 0; i < data.hello_world_len; ++i)
    {
        REQUIRE(s.length() == i);
        s.push_back(data.hello_world[i]);
        REQUIRE(s.length() == i + 1);
    }
    REQUIRE(s.length() == data.hello_world_len);
    REQUIRE(Traits::compare(s.data(), data.hello_world, data.hello_world_len) == 0);

    // remove the last byte
    for (size_t i = 0; i < data.hello_world_len; ++i)
    {
        REQUIRE(s.length() == data.hello_world_len - i);
        s.pop_back();
        REQUIRE(s.length() == data.hello_world_len - i - 1);
    }
    REQUIRE(s.length() == 0u);
    REQUIRE(s.empty());
}

/* insert functions */
TEMPLATE_LIST_TEST_CASE("StoragePassword insert", "[storage_password]", CharTypes)
{
    using CharType = TestType;
    using StorageType = spsl::StoragePassword<CharType>;
    const TestData<CharType> data;
    using Traits = typename StorageType::traits_type;

    // prototype: void insert(size_type index, size_type count, char_type ch)
    StorageType s;
    const CharType ch = data.hello_world[0];
    s.insert(0, 5, ch);
    REQUIRE(s.length() == 5u);
    for (size_t i = 0; i < 5; ++i)
        REQUIRE(s[i] == ch);

    // insert again at the beginning
    const CharType ch2 = data.hello_world[1];
    s.insert(0, 5, ch2);
    REQUIRE(s.length() == 10u);
    for (size_t i = 0; i < 5; ++i)
        REQUIRE(s[i] == ch2);
    for (size_t i = 5; i < 10; ++i)
        REQUIRE(s[i] == ch);

    // insert in the middle
    const CharType ch3 = data.hello_world[2];
    s.insert(5, 10, ch3);
    REQUIRE(s.length() == 20u);
    for (size_t i = 0; i < 5; ++i)
        REQUIRE(s[i] == ch2);
    for (size_t i = 5; i < 15; ++i)
        REQUIRE(s[i] == ch3);
    for (size_t i = 15; i < 20; ++i)
        REQUIRE(s[i] == ch);

    const std::basic_string<CharType> bs(data.blablabla);

    // insert a string
    StorageType str2(s);
    s.insert(15, bs.data(), bs.size());
    REQUIRE(s.length() == 20 + bs.length());

    for (size_t i = 0; i < 5; ++i)
        REQUIRE(s[i] == ch2);
    for (size_t i = 5; i < 15; ++i)
        REQUIRE(s[i] == ch3);
    REQUIRE(Traits::compare(s.data() + 15, data.blablabla, data.blablabla_len) == 0);
    for (size_t i = 15 + data.blablabla_len; i < 20 + data.blablabla_len; ++i)
        REQUIRE(s[i] == ch);

    // again using an iterator range
    s = str2;
    s.insert(15, bs.begin(), bs.end());
    REQUIRE(s.length() == 20 + bs.length());

    for (size_t i = 0; i < 5; ++i)
        REQUIRE(s[i] == ch2);
    for (size_t i = 5; i < 15; ++i)
        REQUIRE(s[i] == ch3);
    REQUIRE(Traits::compare(s.data() + 15, data.blablabla, data.blablabla_len) == 0);
    for (size_t i = 15 + data.blablabla_len; i < 20 + data.blablabla_len; ++i)
        REQUIRE(s[i] == ch);
}

/* std::out_of_range when trying to insert past the end */
TEMPLATE_LIST_TEST_CASE("StoragePassword insert range error", "[storage_password]", CharTypes)
{
    using CharType = TestType;
    using StorageType = spsl::StoragePassword<CharType>;
    const TestData<CharType> data;

    StorageType s;
    const CharType ch = data.hello_world[0];
    s.assign(3, ch);

    // void insert(size_type index, size_type count, char_type ch)
    REQUIRE_THROWS_AS(s.insert(s.size() + 1, 100, ch), std::out_of_range);
    // the content is unchanged
    REQUIRE(s.length() == 3u);
    for (size_t i = 0; i < s.length(); ++i)
        REQUIRE(s[i] == ch);

    // void insert(size_type index, const char_type* s, size_type n)
    REQUIRE_THROWS_AS(s.insert(s.size() + 1, data.hello_world, data.hello_world_len),
                      std::out_of_range);
    // the content is unchanged
    REQUIRE(s.length() == 3u);
    for (size_t i = 0; i < s.length(); ++i)
        REQUIRE(s[i] == ch);

    // void insert(size_type index, InputIt first, InputIt last)
    const std::basic_string<CharType> ref(data.hello_world);
    REQUIRE_THROWS_AS(s.insert(s.size() + 1, ref.begin(), ref.end()), std::out_of_range);
    // the content is unchanged
    REQUIRE(s.length() == 3u);
    for (size_t i = 0; i < s.length(); ++i)
        REQUIRE(s[i] == ch);
}

/* append functions */
TEMPLATE_LIST_TEST_CASE("StoragePassword append", "[storage_password]", CharTypes)
{
    using CharType = TestType;
    using StorageType = spsl::StoragePassword<CharType>;
    const TestData<CharType> data;

    // apply all functions to the array and std::basic_string at the same time and make sure they
    // stay identical
    StorageType s;
    std::basic_string<CharType> ref;

    // void append(const char_type* s, size_type n)
    s.append(data.hello_world, data.hello_world_len);
    ref.append(data.hello_world, data.hello_world_len);
    REQUIRE(ref == s.data());
    REQUIRE(ref.length() == s.length());
    REQUIRE(ref.size() == s.size());

    s.append(data.blablabla, data.blablabla_len);
    ref.append(data.blablabla, data.blablabla_len);
    REQUIRE(ref == s.data());
    REQUIRE(ref.length() == s.length());
    REQUIRE(ref.size() == s.size());

    // void append(size_type count, char_type ch)
    const CharType ch = data.hello_world[0];
    s.append(20, ch);
    ref.append(20, ch);
    REQUIRE(ref == s.data());
    REQUIRE(ref.length() == s.length());
    REQUIRE(ref.size() == s.size());

    // void append(InputIt first, InputIt last)
    const std::basic_string<CharType> bs(data.blablabla);
    s.append(bs.begin(), bs.end());
    ref.append(bs.begin(), bs.end());
    REQUIRE(ref == s.data());
    REQUIRE(ref.length() == s.length());
    REQUIRE(ref.size() == s.size());
}

/* swap function */
TEMPLATE_LIST_TEST_CASE("StoragePassword swap", "[storage_password]", CharTypes)
{
    using CharType = TestType;
    using StorageType = spsl::StoragePassword<CharType>;
    const TestData<CharType> data;
    using Traits = typename StorageType::traits_type;

    StorageType str1, str2;
    // let's start with swapping empty strings...
    REQUIRE(str1.empty());
    REQUIRE(str2.empty());
    str1.swap(str2);
    REQUIRE(str1.empty());
    REQUIRE(str2.empty());

    // assign + swap
    str1.assign(data.hello_world, data.hello_world_len);
    REQUIRE(str1.length() == data.hello_world_len);
    REQUIRE(Traits::compare(str1.data(), data.hello_world, data.hello_world_len) == 0);
    REQUIRE(str2.length() == 0u);
    str1.swap(str2);
    REQUIRE(str2.length() == data.hello_world_len);
    REQUIRE(Traits::compare(str2.data(), data.hello_world, data.hello_world_len) == 0);
    REQUIRE(str1.length() == 0u);

    // swap back
    str1.swap(str2);
    REQUIRE(str1.length() == data.hello_world_len);
    REQUIRE(Traits::compare(str1.data(), data.hello_world, data.hello_world_len) == 0);
    REQUIRE(str2.length() == 0u);

    // now swap 2 non-empty strings
    str1.assign(data.hello_world, data.hello_world_len);
    REQUIRE(str1.length() == data.hello_world_len);
    REQUIRE(Traits::compare(str1.data(), data.hello_world, data.hello_world_len) == 0);
    str2.assign(data.blablabla, data.blablabla_len);
    REQUIRE(str2.length() == data.blablabla_len);
    REQUIRE(Traits::compare(str2.data(), data.blablabla, data.blablabla_len) == 0);
    str2.swap(str1);
    REQUIRE(str2.length() == data.hello_world_len);
    REQUIRE(Traits::compare(str2.data(), data.hello_world, data.hello_world_len) == 0);
    REQUIRE(str1.length() == data.blablabla_len);
    REQUIRE(Traits::compare(str1.data(), data.blablabla, data.blablabla_len) == 0);
}

/* resize function */
TEMPLATE_LIST_TEST_CASE("StoragePassword resize", "[storage_password]", CharTypes)
{
    using CharType = TestType;
    // block size 32
    using StorageType = spsl::StoragePassword<CharType, 32>;
    const TestData<CharType> data;
    using size_type = typename StorageType::size_type;
    const CharType nul = StorageType::nul;

    StorageType s;
    // empty
    REQUIRE(s.size() == 0u);
    REQUIRE(s.length() == 0u);
    const CharType ch = data.hello_world[0];

    // resize + 10 characters
    s.resize(10, ch);
    REQUIRE(s.size() == 10u);
    REQUIRE(s.length() == 10u);
    for (size_t i = 0; i < 10; ++i)
        REQUIRE(s[i] == ch);
    REQUIRE(s[10] == nul);

    // again + 10 characters
    s.resize(20, ch);
    REQUIRE(s.size() == 20u);
    REQUIRE(s.length() == 20u);
    for (size_t i = 0; i < 20; ++i)
        REQUIRE(s[i] == ch);
    REQUIRE(s[20] == nul);

    // shrink 3 characters
    s.resize(17, ch);
    REQUIRE(s.size() == 17u);
    REQUIRE(s.length() == 17u);
    for (size_t i = 0; i < 17; ++i)
        REQUIRE(s[i] == ch);
    REQUIRE(s[17] == nul);

    // shrink_to_fit may reduce the capacity, but not the size
    const size_type oldCapa = s.capacity();
    const size_type oldSize = s.size();
    s.shrink_to_fit();
    REQUIRE(oldCapa <= s.capacity());
    REQUIRE(oldSize == s.size());
}

/* allocation function */
TEMPLATE_LIST_TEST_CASE("StoragePassword realloc", "[storage_password]", CharTypes)
{
    using CharType = TestType;
    // block size 32
    using StorageType = spsl::StoragePassword<CharType, 32>;
    using size_type = typename StorageType::size_type;
    const size_type block_size = StorageType::block_size();
    const TestData<CharType> data;
    const CharType nul = StorageType::nul;
    const CharType ch(data.hello_world[2]);

    StorageType s;
    REQUIRE(s.size() == 0u);
    REQUIRE(s.capacity() == 0u);

    // requesting < block_size allocates a full block
    s.reserve(block_size - 1);
    REQUIRE(s.size() == 0u);
    REQUIRE(s.capacity() == block_size);

    // now request more than 1 block
    s.reserve(block_size + 1);
    REQUIRE(s.size() == 0u);
    REQUIRE(s.capacity() == 2 * block_size);

    // the capacity won't change if we assign a string that fits
    s.resize(block_size / 2, ch);
    REQUIRE(s.size() == block_size / 2);
    REQUIRE(s.capacity() == 2 * block_size);
    for (size_type i = 0; i < s.size(); ++i)
        REQUIRE(s[i] == ch);
    REQUIRE(s[s.size()] == nul);

    // shrink_to_fit now reduces the size to 1 block
    s.shrink_to_fit();
    REQUIRE(s.size() == block_size / 2);
    REQUIRE(s.capacity() == block_size);
    for (size_type i = 0; i < s.size(); ++i)
        REQUIRE(s[i] == ch);
    REQUIRE(s[s.size()] == nul);

    // clear + shrink frees all memory
    s.clear();
    s.shrink_to_fit();
    REQUIRE(s.size() == 0u);
    REQUIRE(s.capacity() == 0u);
}

/* wiping memory */
TEMPLATE_LIST_TEST_CASE("StoragePassword wiping", "[storage_password]", CharTypes)
{
    // verify that the buffer is wiped automatically upon destruction
    // -> we do this by providing a custom allocator that performs these checks

    using CharType = TestType;
    using StorageType = spsl::StoragePassword<CharType, 32, WipeCheckAllocator<CharType>>;
    using size_type = typename StorageType::size_type;
    const TestData<CharType> data;
    const CharType ch = data.blablabla[0];
    const CharType nul = StorageType::nul;

    StorageType s;
    s.assign(data.hello_world, data.hello_world_len);
    REQUIRE(!s.empty());

    // removing the last character will wipe it's storage
    size_type index = s.size() - 1;
    REQUIRE(s[index] != nul);
    s.pop_back();
    REQUIRE(s[index] == nul);

    // resize to shrink, then make sure the rest was wiped
    s.resize(index / 2, ch);
    REQUIRE(s.size() == index / 2);
    for (size_type i = s.size(); i < index; ++i)
        REQUIRE(s[i] == nul);

    // great, now clear the whole string
    s.clear();
    REQUIRE(s.capacity() > 0u);
    for (size_type i = 0; i < s.capacity(); ++i)
        REQUIRE(s[i] == nul);

    // verify that the buffer is wiped automatically upon destruction
    {
        StorageType s2;
        for (size_t i = 0; i < 100; ++i)
            s2.append(data.blablabla, data.blablabla_len);
    }
}

/* replace functions */
TEMPLATE_LIST_TEST_CASE("StoragePassword replace", "[storage_password]", CharTypes)
{
    using CharType = TestType;
    using StorageType = spsl::StoragePassword<CharType, 32>;
    using Traits = typename StorageType::traits_type;
    const TestData<CharType> data;
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
        REQUIRE(s.size() == ref.size());
        REQUIRE(Traits::compare(s.data(), ref.data(), ref.size()) == 0);
        // with count = 0
        s.assign(data.hello_world, data.hello_world_len);
        s.replace(0, 0, data.blablabla, data.blablabla_len);
        ref = data.blablabla;
        ref += data.hello_world;
        REQUIRE(s.size() == ref.size());
        REQUIRE(Traits::compare(s.data(), ref.data(), ref.size()) == 0);

        // replace in the middle
        s.assign(data.hello_world, data.hello_world_len);
        s.replace(3, 6, data.blablabla, data.blablabla_len);
        ref.assign(data.hello_world, 3);
        ref += data.blablabla;
        ref += data.hello_world + 9;
        REQUIRE(s.size() == ref.size());
        REQUIRE(Traits::compare(s.data(), ref.data(), ref.size()) == 0);

        // replace at the end
        s.assign(data.hello_world, data.hello_world_len);
        s.replace(6, 6, data.blablabla, data.blablabla_len);
        ref.assign(data.hello_world, 6);
        ref += data.blablabla;
        REQUIRE(s.size() == ref.size());
        REQUIRE(Traits::compare(s.data(), ref.data(), ref.size()) == 0);
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
        REQUIRE(s.size() == ref.size());
        REQUIRE(Traits::compare(s.data(), ref.data(), ref.size()) == 0);
        // with count = 0
        s.assign(data.hello_world, data.hello_world_len);
        s.replace(0, 0, 10, b);
        ref.assign(10, b);
        ref += data.hello_world;
        REQUIRE(s.size() == ref.size());
        REQUIRE(Traits::compare(s.data(), ref.data(), ref.size()) == 0);

        // replace in the middle
        s.assign(data.hello_world, data.hello_world_len);
        s.replace(3, 6, 20, b);
        ref.assign(data.hello_world, 3);
        ref.append(20, b);
        ref += data.hello_world + 9;
        REQUIRE(s.size() == ref.size());
        REQUIRE(Traits::compare(s.data(), ref.data(), ref.size()) == 0);

        // replace at the end
        s.assign(data.hello_world, data.hello_world_len);
        s.replace(6, 6, 13, b);
        ref.assign(data.hello_world, 6);
        ref.append(13, b);
        REQUIRE(s.size() == ref.size());
        REQUIRE(Traits::compare(s.data(), ref.data(), ref.size()) == 0);
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
        REQUIRE(s.size() == ref.size());
        REQUIRE(Traits::compare(s.data(), ref.data(), ref.size()) == 0);
        // with count = 0
        s.assign(data.hello_world, data.hello_world_len);
        s.replace(0, 0, vec.cbegin(), vec.cend());
        ref.assign(12, l);
        ref += data.hello_world;
        REQUIRE(s.size() == ref.size());
        REQUIRE(Traits::compare(s.data(), ref.data(), ref.size()) == 0);

        // replace in the middle
        s.assign(data.hello_world, data.hello_world_len);
        s.replace(3, 6, vec.begin(), vec.begin() + 2);
        ref.assign(data.hello_world, 3);
        ref.append(2, l);
        ref += data.hello_world + 9;
        REQUIRE(s.size() == ref.size());
        REQUIRE(Traits::compare(s.data(), ref.data(), ref.size()) == 0);

        // replace at the end
        s.assign(data.hello_world, data.hello_world_len);
        s.replace(6, 6, vec.rbegin(), vec.rend());
        ref.assign(data.hello_world, 6);
        ref.append(12, l);
        REQUIRE(s.size() == ref.size());
        REQUIRE(Traits::compare(s.data(), ref.data(), ref.size()) == 0);
    }
}

/* copy & move tests */
TEMPLATE_LIST_TEST_CASE("StoragePassword copy and move", "[storage_password]", CharTypes)
{
    using CharType = TestType;
    using StorageType = spsl::StoragePassword<CharType>;
    const TestData<CharType> data;

    // create 2 distinct page allocator instances
    spsl::SensitivePageAllocator alloc1, alloc2;
    // use them in 2 different strings
    StorageType string1{ spsl::SensitiveSegmentAllocator<CharType>(alloc1) };
    string1.assign(data.hello_world, data.hello_world_len);
    StorageType string2{ spsl::SensitiveSegmentAllocator<CharType>(alloc2) };
    string2.assign(data.hello_world, data.hello_world_len);

    // same content, different allocators
    REQUIRE(compareStrings(string1.data(), string2.data()) == 0);
    REQUIRE(string1.getAllocator().pageAllocator() == &alloc1);
    REQUIRE(string2.getAllocator().pageAllocator() == &alloc2);

    // swap: allocators are swapped, too
    std::swap(string1, string2);
    REQUIRE(compareStrings(string1.data(), string2.data()) == 0);
    REQUIRE(string1.getAllocator().pageAllocator() == &alloc2);
    REQUIRE(string2.getAllocator().pageAllocator() == &alloc1);

    // change the content and swap back
    string1.assign(data.blablabla, data.blablabla_len);
    REQUIRE(compareStrings(string1.data(), data.blablabla) == 0);
    REQUIRE(compareStrings(string2.data(), data.hello_world) == 0);
    REQUIRE(compareStrings(string1.data(), string2.data()) != 0);
    std::swap(string1, string2);
    REQUIRE(compareStrings(string1.data(), data.hello_world) == 0);
    REQUIRE(compareStrings(string2.data(), data.blablabla) == 0);
    REQUIRE(string1.getAllocator().pageAllocator() == &alloc1);
    REQUIRE(string2.getAllocator().pageAllocator() == &alloc2);

    // using the copy or move constructor copies the allocator
    StorageType string3(string1);
    StorageType string4(std::move(string1));
    REQUIRE(string1.getAllocator().pageAllocator() == &alloc1);
    REQUIRE(string3.getAllocator().pageAllocator() == &alloc1);
    REQUIRE(string4.getAllocator().pageAllocator() == &alloc1);
    // copy assignment doesn't
    string3 = string2;
    REQUIRE(string3.getAllocator().pageAllocator() == &alloc1);

    // moving swaps the allocator
    auto defaultAlloc = &spsl::SensitivePageAllocator::getDefaultInstance();
    StorageType string5;
    REQUIRE(string5.getAllocator().pageAllocator() == defaultAlloc);
    string5 = std::move(string1);
    REQUIRE(string5.getAllocator().pageAllocator() == &alloc1);
    REQUIRE(string1.getAllocator().pageAllocator() == defaultAlloc);
    string5.swap(string1);
    REQUIRE(string1.getAllocator().pageAllocator() == &alloc1);
    REQUIRE(string5.getAllocator().pageAllocator() == defaultAlloc);
}

/* erase function */
TEMPLATE_LIST_TEST_CASE("StoragePassword erase", "[storage_password]", CharTypes)
{
    using CharType = TestType;
    using StorageType = spsl::StoragePassword<CharType>;
    using Traits = typename StorageType::traits_type;
    const TestData<CharType> data;
    using RefType = std::basic_string<CharType>;

    // void erase(size_type index, size_type count)
    StorageType s;
    RefType ref;
    s.assign(data.hello_world, data.hello_world_len);
    ref = data.hello_world;
    REQUIRE(s.size() == ref.size());
    REQUIRE(Traits::compare(s.data(), ref.data(), ref.size()) == 0);

    s.erase(0, 1);
    ref.erase(0, 1);
    REQUIRE(s.size() == ref.size());
    REQUIRE(Traits::compare(s.data(), ref.data(), ref.size()) == 0);

    s.erase(5, 3);
    ref.erase(5, 3);
    REQUIRE(s.size() == ref.size());
    REQUIRE(Traits::compare(s.data(), ref.data(), ref.size()) == 0);

    s.erase(0, s.size());
    ref.erase(0, ref.size());
    REQUIRE(s.size() == ref.size());
    REQUIRE(Traits::compare(s.data(), ref.data(), ref.size()) == 0);
    REQUIRE(s.empty());
}
