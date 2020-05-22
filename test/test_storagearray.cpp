/**
 * @file	Special Purpose Strings Library: test_storagearray.cpp
 * @author	Daniel Evers
 * @brief   StorageArray unit tests
 * @license MIT
 */

#include <tuple>

#include "doctest.h"

#include "spsl/storage_array.hpp"
#include "testdata.hpp"


// all character types we want to test
using CharTypes = std::tuple<char, wchar_t, gsl::byte>;


/* check if size is checkable on type at run/compile time */
TEST_CASE_TEMPLATE_DEFINE("StorageArray static size", CharType, StorageArray_static_size)
{
    using ArrayType = spsl::StorageArray<CharType, 64>;
    using size_type = typename ArrayType::size_type;

    REQUIRE(ArrayType::max_size() == 64u);
    static_assert(ArrayType::max_size() == 64,
                  "Size of StorageArray has to be available at compile time");

    // and check the size requirements (there may be some padding, but max. 1 word)
    constexpr size_t minSize = sizeof(size_type) + sizeof(CharType) * (ArrayType::max_size() + 1);
    constexpr size_t maxSize = minSize + sizeof(size_t);
    REQUIRE(minSize <= sizeof(ArrayType));
    REQUIRE(maxSize >= sizeof(ArrayType));
    //   static_assert(minSize <= sizeof(ArrayType) && sizeof(ArrayType) <= maxSize,
    //                 "Unexpected object size?");
}
TEST_CASE_TEMPLATE_APPLY(StorageArray_static_size, CharTypes);

/* constructor tests */
TEST_CASE_TEMPLATE_DEFINE("StorageArray constructors", CharType, StorageArray_constructors)
{
    using ArrayType = spsl::StorageArray<CharType, 64>;
    const CharType nul = ArrayType::nul();

    const ArrayType s1;
    REQUIRE(s1.capacity() == 64u);
    REQUIRE(s1.max_size() == 64u);

    REQUIRE(s1.empty());
    REQUIRE(s1.length() == 0u);
    REQUIRE(s1.length() == s1.size());
    REQUIRE(s1.capacity_left() == s1.max_size());
    // valid content?
    REQUIRE(s1.data() != nullptr);
    REQUIRE(s1.data()[0] == nul);

    // copy & move constructor are available
    ArrayType s2(s1);
    ArrayType s3(std::move(s2));
}
TEST_CASE_TEMPLATE_APPLY(StorageArray_constructors, CharTypes);

/* assignment functions */
TEST_CASE_TEMPLATE_DEFINE("StorageArray assignment", CharType, StorageArray_assignment)
{
    using ArrayType = spsl::StorageArray<CharType, 64>;
    const TestData<CharType> data;
    using Traits = typename ArrayType::traits_type;

    ArrayType arr;
    // assign a string
    arr.assign(data.hello_world, data.hello_world_len);
    REQUIRE(arr.length() == Traits::length(arr.data()));
    REQUIRE(arr.length() == data.hello_world_len);
    REQUIRE(Traits::compare(arr.data(), data.hello_world, data.hello_world_len) == 0);

    // assign something else
    arr.assign(data.blablabla, data.blablabla_len);
    REQUIRE(arr.length() == Traits::length(arr.data()));
    REQUIRE(arr.length() == data.blablabla_len);
    REQUIRE(Traits::compare(arr.data(), data.blablabla, data.blablabla_len) == 0);

    // assign a repeated character
    const CharType ch = data.hello_world[0];
    arr.assign(33, ch);
    REQUIRE(arr.length() == 33u);
    for (size_t i = 0; i < 33; ++i)
    {
        REQUIRE(arr[i] == ch);
    }

    // assign an iterator range - we use std::basic_string here
    const std::basic_string<CharType> bs(data.hello_world);
    arr.assign(bs.begin(), bs.end());
    REQUIRE(arr.length() == bs.length());
    REQUIRE(Traits::compare(arr.data(), data.hello_world, data.hello_world_len) == 0);
    REQUIRE(bs == arr.data());
}
TEST_CASE_TEMPLATE_APPLY(StorageArray_assignment, CharTypes);

/* push_back/pop_back functions */
TEST_CASE_TEMPLATE_DEFINE("StorageArray push and pop", CharType, StorageArray_push_pop)
{
    using ArrayType = spsl::StorageArray<CharType, 64>;
    const TestData<CharType> data;
    using Traits = typename ArrayType::traits_type;

    ArrayType arr;
    // append a string byte for byte
    for (size_t i = 0; i < data.hello_world_len; ++i)
    {
        REQUIRE(arr.length() == i);
        arr.push_back(data.hello_world[i]);
        REQUIRE(arr.length() == i + 1);
    }
    REQUIRE(arr.length() == data.hello_world_len);
    REQUIRE(Traits::compare(arr.data(), data.hello_world, data.hello_world_len) == 0);

    // remove the last byte
    for (size_t i = 0; i < data.hello_world_len; ++i)
    {
        REQUIRE(arr.length() == data.hello_world_len - i);
        arr.pop_back();
        REQUIRE(arr.length() == data.hello_world_len - i - 1);
    }
    REQUIRE(arr.length() == 0u);
    REQUIRE(arr.empty());
}
TEST_CASE_TEMPLATE_APPLY(StorageArray_push_pop, CharTypes);

/* insert functions */
TEST_CASE_TEMPLATE_DEFINE("StorageArray insert", CharType, StorageArray_insert)
{
    using ArrayType = spsl::StorageArray<CharType, 64>;
    const TestData<CharType> data;
    using Traits = typename ArrayType::traits_type;

    // prototype: void insert(size_type index, size_type count, char_type ch)
    ArrayType arr;
    const CharType ch = data.hello_world[0];
    arr.insert(0, 5, ch);
    REQUIRE(arr.length() == 5u);
    for (size_t i = 0; i < 5; ++i)
        REQUIRE(arr[i] == ch);

    // insert again at the beginning
    const CharType ch2 = data.hello_world[1];
    arr.insert(0, 5, ch2);
    REQUIRE(arr.length() == 10u);
    for (size_t i = 0; i < 5; ++i)
        REQUIRE(arr[i] == ch2);
    for (size_t i = 5; i < 10; ++i)
        REQUIRE(arr[i] == ch);

    // insert in the middle
    const CharType ch3 = data.hello_world[2];
    arr.insert(5, 10, ch3);
    REQUIRE(arr.length() == 20u);
    for (size_t i = 0; i < 5; ++i)
        REQUIRE(arr[i] == ch2);
    for (size_t i = 5; i < 15; ++i)
        REQUIRE(arr[i] == ch3);
    for (size_t i = 15; i < 20; ++i)
        REQUIRE(arr[i] == ch);

    const std::basic_string<CharType> bs(data.blablabla);

    // insert a string
    ArrayType arr2(arr);
    arr.insert(15, bs.data(), bs.size());
    REQUIRE(arr.length() == 20 + bs.length());

    for (size_t i = 0; i < 5; ++i)
        REQUIRE(arr[i] == ch2);
    for (size_t i = 5; i < 15; ++i)
        REQUIRE(arr[i] == ch3);
    REQUIRE(Traits::compare(arr.data() + 15, data.blablabla, data.blablabla_len) == 0);
    for (size_t i = 15 + data.blablabla_len; i < 20 + data.blablabla_len; ++i)
        REQUIRE(arr[i] == ch);

    // again using an iterator range
    arr = arr2;
    arr.insert(15, bs.begin(), bs.end());
    REQUIRE(arr.length() == 20 + bs.length());

    for (size_t i = 0; i < 5; ++i)
        REQUIRE(arr[i] == ch2);
    for (size_t i = 5; i < 15; ++i)
        REQUIRE(arr[i] == ch3);
    REQUIRE(Traits::compare(arr.data() + 15, data.blablabla, data.blablabla_len) == 0);
    for (size_t i = 15 + data.blablabla_len; i < 20 + data.blablabla_len; ++i)
        REQUIRE(arr[i] == ch);
}
TEST_CASE_TEMPLATE_APPLY(StorageArray_insert, CharTypes);

/* append functions */
TEST_CASE_TEMPLATE_DEFINE("StorageArray append", CharType, StorageArray_append)
{
    using ArrayType = spsl::StorageArray<CharType, 64>;
    const TestData<CharType> data;

    // apply all functions to the array and std::basic_string at the same time and make sure they
    // stay identical
    ArrayType arr;
    std::basic_string<CharType> ref;

    // void append(const char_type* s, size_type n)
    arr.append(data.hello_world, data.hello_world_len);
    ref.append(data.hello_world, data.hello_world_len);
    REQUIRE(ref == arr.data());
    REQUIRE(ref.length() == arr.length());
    REQUIRE(ref.size() == arr.size());

    arr.append(data.blablabla, data.blablabla_len);
    ref.append(data.blablabla, data.blablabla_len);
    REQUIRE(ref == arr.data());
    REQUIRE(ref.length() == arr.length());
    REQUIRE(ref.size() == arr.size());

    // void append(size_type count, char_type ch)
    const CharType ch = data.hello_world[0];
    arr.append(20, ch);
    ref.append(20, ch);
    REQUIRE(ref == arr.data());
    REQUIRE(ref.length() == arr.length());
    REQUIRE(ref.size() == arr.size());

    // void append(InputIt first, InputIt last)
    const std::basic_string<CharType> bs(data.blablabla);
    arr.append(bs.begin(), bs.end());
    ref.append(bs.begin(), bs.end());
    REQUIRE(ref == arr.data());
    REQUIRE(ref.length() == arr.length());
    REQUIRE(ref.size() == arr.size());
}
TEST_CASE_TEMPLATE_APPLY(StorageArray_append, CharTypes);

/* swap function */
TEST_CASE_TEMPLATE_DEFINE("StorageArray swap", CharType, StorageArray_swap)
{
    using ArrayType = spsl::StorageArray<CharType, 64>;
    const TestData<CharType> data;
    using Traits = typename ArrayType::traits_type;

    ArrayType arr1, arr2;
    // let's start with swapping empty strings...
    REQUIRE(arr1.empty());
    REQUIRE(arr2.empty());
    arr1.swap(arr2);
    REQUIRE(arr1.empty());
    REQUIRE(arr2.empty());

    // assign + swap
    arr1.assign(data.hello_world, data.hello_world_len);
    REQUIRE(arr1.length() == data.hello_world_len);
    REQUIRE(Traits::compare(arr1.data(), data.hello_world, data.hello_world_len) == 0);
    REQUIRE(arr2.length() == 0u);
    arr1.swap(arr2);
    REQUIRE(arr2.length() == data.hello_world_len);
    REQUIRE(Traits::compare(arr2.data(), data.hello_world, data.hello_world_len) == 0);
    REQUIRE(arr1.length() == 0u);

    // swap back
    arr1.swap(arr2);
    REQUIRE(arr1.length() == data.hello_world_len);
    REQUIRE(Traits::compare(arr1.data(), data.hello_world, data.hello_world_len) == 0);
    REQUIRE(arr2.length() == 0u);

    // now swap 2 non-empty strings
    arr1.assign(data.hello_world, data.hello_world_len);
    REQUIRE(arr1.length() == data.hello_world_len);
    REQUIRE(Traits::compare(arr1.data(), data.hello_world, data.hello_world_len) == 0);
    arr2.assign(data.blablabla, data.blablabla_len);
    REQUIRE(arr2.length() == data.blablabla_len);
    REQUIRE(Traits::compare(arr2.data(), data.blablabla, data.blablabla_len) == 0);
    arr2.swap(arr1);
    REQUIRE(arr2.length() == data.hello_world_len);
    REQUIRE(Traits::compare(arr2.data(), data.hello_world, data.hello_world_len) == 0);
    REQUIRE(arr1.length() == data.blablabla_len);
    REQUIRE(Traits::compare(arr1.data(), data.blablabla, data.blablabla_len) == 0);
}
TEST_CASE_TEMPLATE_APPLY(StorageArray_swap, CharTypes);

/* resize function */
TEST_CASE_TEMPLATE_DEFINE("StorageArray resize", CharType, StorageArray_resize)
{
    using ArrayType = spsl::StorageArray<CharType, 64>;
    const TestData<CharType> data;
    const CharType nul = ArrayType::nul();

    // we'll stay within max_size() here
    ArrayType arr;
    // empty
    REQUIRE(arr.size() == 0u);
    REQUIRE(arr.length() == 0u);
    REQUIRE(arr.max_size() == 64u);
    REQUIRE(arr.capacity_left() == 64u);
    const CharType ch = data.hello_world[0];

    // resize + 10 characters
    arr.resize(10, ch);
    REQUIRE(arr.size() == 10u);
    REQUIRE(arr.length() == 10u);
    REQUIRE(arr.max_size() == 64u);
    REQUIRE(arr.capacity_left() == 54u);
    for (size_t i = 0; i < 10; ++i)
        REQUIRE(arr[i] == ch);
    REQUIRE(arr[10] == nul);

    // again + 10 characters
    arr.resize(20, ch);
    REQUIRE(arr.size() == 20u);
    REQUIRE(arr.length() == 20u);
    REQUIRE(arr.max_size() == 64u);
    REQUIRE(arr.capacity_left() == 44u);
    for (size_t i = 0; i < 20; ++i)
        REQUIRE(arr[i] == ch);
    REQUIRE(arr[20] == nul);

    // shrink 3 characters
    arr.resize(17, ch);
    REQUIRE(arr.size() == 17u);
    REQUIRE(arr.length() == 17u);
    REQUIRE(arr.max_size() == 64u);
    REQUIRE(arr.capacity_left() == 47u);
    for (size_t i = 0; i < 17; ++i)
        REQUIRE(arr[i] == ch);
    REQUIRE(arr[17] == nul);

    // quick check: this no-op doesn't kill any kittens...
    arr.shrink_to_fit();
}
TEST_CASE_TEMPLATE_APPLY(StorageArray_resize, CharTypes);

/* truncating when exceeding max_size() during assign() */
TEST_CASE_TEMPLATE_DEFINE("StorageArray assignment truncation", CharType, StorageArray_assign_trunc)
{
    // this variant definitely won't throw
    using ArrayType = spsl::StorageArray<CharType, 64, spsl::policy::overflow::Truncate>;
    const TestData<CharType> data;

    // I can reserve as much as I want - it'll be ignored
    const size_t too_large = 1000;
    ArrayType arr;
    REQUIRE(too_large > arr.max_size());
    REQUIRE_NOTHROW(arr.reserve(1000));

    // I can assign as much as I want - it'll be truncated
    const CharType ch = data.hello_world[0];
    REQUIRE_NOTHROW(arr.assign(too_large, ch));
    REQUIRE(arr.length() == arr.max_size());
    REQUIRE(arr.size() == arr.max_size());
    REQUIRE(arr.capacity_left() == 0u);
    for (size_t i = 0; i < arr.length(); ++i)
        REQUIRE(arr[i] == ch);

    // same test, but assign from a character array
    std::basic_string<CharType> ref;
    while (ref.size() < too_large)
        ref += data.hello_world;
    arr.clear();
    REQUIRE(ref.size() >= too_large);
    REQUIRE(ref.size() > arr.max_size());
    REQUIRE_NOTHROW(arr.assign(ref.data(), ref.size()));
    REQUIRE(arr.length() == arr.max_size());
    REQUIRE(arr.size() == arr.max_size());
    REQUIRE(arr.capacity_left() == 0u);
    for (size_t i = 0; i < arr.length(); ++i)
        REQUIRE(arr[i] == ref[i]);

    // and again using an iterator
    arr.clear();
    REQUIRE(ref.size() >= too_large);
    REQUIRE(ref.size() > arr.max_size());
    REQUIRE_NOTHROW(arr.assign(ref.begin(), ref.end()));
    REQUIRE(arr.length() == arr.max_size());
    REQUIRE(arr.size() == arr.max_size());
    REQUIRE(arr.capacity_left() == 0u);
    for (size_t i = 0; i < arr.length(); ++i)
        REQUIRE(arr[i] == ref[i]);

    // using push_back()
    arr.clear();
    for (auto c : ref)
    {
        REQUIRE_NOTHROW(arr.push_back(c));
    }
    REQUIRE(arr.length() == arr.max_size());
    REQUIRE(arr.size() == arr.max_size());
    REQUIRE(arr.capacity_left() == 0u);
    for (size_t i = 0; i < arr.length(); ++i)
        REQUIRE(arr[i] == ref[i]);
}
TEST_CASE_TEMPLATE_APPLY(StorageArray_assign_trunc, CharTypes);

/* std::length_error when exceeding max_size() during assign() */
TEST_CASE_TEMPLATE_DEFINE("StorageArray assignment length errors", CharType,
                          StorageArray_assign_length_err)
{
    // this variant definitely will throw
    using ArrayType = spsl::StorageArray<CharType, 64, spsl::policy::overflow::Throw>;
    const TestData<CharType> data;

    ArrayType arr;
    const CharType ch = data.hello_world[0];
    arr.assign(3, ch);

    // I can't reserve as much as I want
    const size_t too_large = 1000;
    REQUIRE(too_large > arr.max_size());
    REQUIRE_THROWS_AS(arr.reserve(1000), std::length_error);
    // the content is unchanged
    REQUIRE(arr.length() == 3u);
    for (size_t i = 0; i < arr.length(); ++i)
        REQUIRE(arr[i] == ch);

    // I can't assign as much as I want
    REQUIRE_THROWS_AS(arr.assign(too_large, ch), std::length_error);
    // the content is unchanged
    REQUIRE(arr.length() == 3u);
    for (size_t i = 0; i < arr.length(); ++i)
        REQUIRE(arr[i] == ch);

    // same test, but assign from a character array
    std::basic_string<CharType> ref;
    while (ref.size() < too_large)
        ref += data.hello_world;
    REQUIRE_THROWS_AS(arr.assign(ref.data(), ref.size()), std::length_error);
    // the content is unchanged
    REQUIRE(arr.length() == 3u);
    for (size_t i = 0; i < arr.length(); ++i)
        REQUIRE(arr[i] == ch);

    // and again using an iterator
    REQUIRE_THROWS_AS(arr.assign(ref.begin(), ref.end()), std::length_error);
    // the content is unchanged
    REQUIRE(arr.length() == 3u);
    for (size_t i = 0; i < arr.length(); ++i)
        REQUIRE(arr[i] == ch);

    // using push_back()
    arr.clear();
    for (size_t i = 0; i < arr.max_size(); ++i)
        REQUIRE_NOTHROW(arr.push_back(ch));
    REQUIRE_THROWS_AS(arr.push_back(ch), std::length_error);
    // the content is unchanged
    REQUIRE(arr.length() == arr.max_size());
    for (size_t i = 0; i < arr.length(); ++i)
        REQUIRE(arr[i] == ch);
}
TEST_CASE_TEMPLATE_APPLY(StorageArray_assign_length_err, CharTypes);

/* truncating when exceeding max_size() during insert() */
TEST_CASE_TEMPLATE_DEFINE("StorageArray insert truncation", CharType, StorageArray_insert_trunc)
{
    // this variant definitely won't throw
    using ArrayType = spsl::StorageArray<CharType, 64, spsl::policy::overflow::Truncate>;
    const TestData<CharType> data;

    const CharType ch = data.hello_world[0];
    ArrayType arr;

    // insert too many characters
    arr.clear();
    arr.assign(arr.max_size() - 1, ch);
    REQUIRE_NOTHROW(arr.insert(0, 100, ch));
    REQUIRE(arr.length() == arr.max_size());
    REQUIRE(arr.size() == arr.max_size());
    REQUIRE(arr.capacity_left() == 0u);
    for (size_t i = 0; i < arr.length(); ++i)
        REQUIRE(arr[i] == ch);

    // insert a string that's too long
    arr.clear();
    const size_t n = 5;
    arr.assign(arr.max_size() - n, ch);
    REQUIRE_NOTHROW(arr.insert(arr.length(), data.hello_world, data.hello_world_len));
    REQUIRE(arr.length() == arr.max_size());
    REQUIRE(arr.size() == arr.max_size());
    REQUIRE(arr.capacity_left() == 0u);
    for (size_t i = 0; i < arr.length() - n; ++i)
        REQUIRE(arr[i] == ch);
    for (size_t i = 0; i < n; ++i)
        REQUIRE(arr[arr.length() - n + i] == data.hello_world[i]);

    // and again using an iterator
    arr.clear();
    arr.assign(arr.max_size() - n, ch);
    std::basic_string<CharType> ref(data.hello_world, data.hello_world_len);
    REQUIRE_NOTHROW(arr.insert(arr.length(), ref.begin(), ref.end()));
    REQUIRE(arr.length() == arr.max_size());
    REQUIRE(arr.size() == arr.max_size());
    REQUIRE(arr.capacity_left() == 0u);
    for (size_t i = 0; i < arr.length() - n; ++i)
        REQUIRE(arr[i] == ch);
    for (size_t i = 0; i < n; ++i)
        REQUIRE(arr[arr.length() - n + i] == data.hello_world[i]);
}
TEST_CASE_TEMPLATE_APPLY(StorageArray_insert_trunc, CharTypes);

/* std::length_error when exceeding max_size() during insert() */
TEST_CASE_TEMPLATE_DEFINE("StorageArray insert length error", CharType,
                          StorageArray_insert_length_err)
{
    // this variant definitely will throw
    using ArrayType = spsl::StorageArray<CharType, 64, spsl::policy::overflow::Throw>;
    const TestData<CharType> data;

    ArrayType arr;
    const CharType ch = data.hello_world[0];
    arr.assign(3, ch);

    // insert too many characters
    REQUIRE_THROWS_AS(arr.insert(0, 100, ch), std::length_error);
    // the content is unchanged
    REQUIRE(arr.length() == 3u);
    for (size_t i = 0; i < arr.length(); ++i)
        REQUIRE(arr[i] == ch);

    // insert a string that's too long
    const std::basic_string<CharType> ref(arr.max_size(), ch);
    REQUIRE_THROWS_AS(arr.insert(arr.length(), ref.data(), ref.size()), std::length_error);
    // the content is unchanged
    REQUIRE(arr.length() == 3u);
    for (size_t i = 0; i < arr.length(); ++i)
        REQUIRE(arr[i] == ch);

    // and again using an iterator
    REQUIRE_THROWS_AS(arr.insert(arr.length(), ref.begin(), ref.end()), std::length_error);
    // the content is unchanged
    REQUIRE(arr.length() == 3u);
    for (size_t i = 0; i < arr.length(); ++i)
        REQUIRE(arr[i] == ch);
}
TEST_CASE_TEMPLATE_APPLY(StorageArray_insert_length_err, CharTypes);

/* std::out_of_range when trying to insert past the end */
TEST_CASE_TEMPLATE_DEFINE("StorageArray insert range error", CharType,
                          StorageArray_insert_range_err)
{
    using ArrayType = spsl::StorageArray<CharType, 64, spsl::policy::overflow::Truncate>;
    const TestData<CharType> data;

    ArrayType arr;
    const CharType ch = data.hello_world[0];
    arr.assign(3, ch);

    // void insert(size_type index, size_type count, char_type ch)
    REQUIRE_THROWS_AS(arr.insert(arr.size() + 1, 100, ch), std::out_of_range);
    // the content is unchanged
    REQUIRE(arr.length() == 3u);
    for (size_t i = 0; i < arr.length(); ++i)
        REQUIRE(arr[i] == ch);

    // void insert(size_type index, const char_type* s, size_type n)
    REQUIRE_THROWS_AS(arr.insert(arr.size() + 1, data.hello_world, data.hello_world_len),
                      std::out_of_range);
    // the content is unchanged
    REQUIRE(arr.length() == 3u);
    for (size_t i = 0; i < arr.length(); ++i)
        REQUIRE(arr[i] == ch);

    // void insert(size_type index, InputIt first, InputIt last)
    const std::basic_string<CharType> ref(data.hello_world);
    REQUIRE_THROWS_AS(arr.insert(arr.size() + 1, ref.begin(), ref.end()), std::out_of_range);
    // the content is unchanged
    REQUIRE(arr.length() == 3u);
    for (size_t i = 0; i < arr.length(); ++i)
        REQUIRE(arr[i] == ch);
}
TEST_CASE_TEMPLATE_APPLY(StorageArray_insert_range_err, CharTypes);

/* replace function */
TEST_CASE_TEMPLATE_DEFINE("StorageArray replace", CharType, StorageArray_replace)
{
    using ArrayType = spsl::StorageArray<CharType, 64>;
    using Traits = typename ArrayType::traits_type;
    const TestData<CharType> data;
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
        REQUIRE(arr.size() == ref.size());
        REQUIRE(Traits::compare(arr.data(), ref.data(), ref.size()) == 0);
        // with count = 0
        arr.assign(data.hello_world, data.hello_world_len);
        arr.replace(0, 0, data.blablabla, data.blablabla_len);
        ref = data.blablabla;
        ref += data.hello_world;
        REQUIRE(arr.size() == ref.size());
        REQUIRE(Traits::compare(arr.data(), ref.data(), ref.size()) == 0);

        // replace in the middle
        arr.assign(data.hello_world, data.hello_world_len);
        arr.replace(3, 6, data.blablabla, data.blablabla_len);
        ref.assign(data.hello_world, 3);
        ref += data.blablabla;
        ref += data.hello_world + 9;
        REQUIRE(arr.size() == ref.size());
        REQUIRE(Traits::compare(arr.data(), ref.data(), ref.size()) == 0);

        // replace at the end
        arr.assign(data.hello_world, data.hello_world_len);
        arr.replace(6, 6, data.blablabla, data.blablabla_len);
        ref.assign(data.hello_world, 6);
        ref += data.blablabla;
        REQUIRE(arr.size() == ref.size());
        REQUIRE(Traits::compare(arr.data(), ref.data(), ref.size()) == 0);
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
        REQUIRE(arr.size() == ref.size());
        REQUIRE(Traits::compare(arr.data(), ref.data(), ref.size()) == 0);
        // with count = 0
        arr.assign(data.hello_world, data.hello_world_len);
        arr.replace(0, 0, 10, b);
        ref.assign(10, b);
        ref += data.hello_world;
        REQUIRE(arr.size() == ref.size());
        REQUIRE(Traits::compare(arr.data(), ref.data(), ref.size()) == 0);

        // replace in the middle
        arr.assign(data.hello_world, data.hello_world_len);
        arr.replace(3, 6, 20, b);
        ref.assign(data.hello_world, 3);
        ref.append(20, b);
        ref += data.hello_world + 9;
        REQUIRE(arr.size() == ref.size());
        REQUIRE(Traits::compare(arr.data(), ref.data(), ref.size()) == 0);

        // replace at the end
        arr.assign(data.hello_world, data.hello_world_len);
        arr.replace(6, 6, 13, b);
        ref.assign(data.hello_world, 6);
        ref.append(13, b);
        REQUIRE(arr.size() == ref.size());
        REQUIRE(Traits::compare(arr.data(), ref.data(), ref.size()) == 0);
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
        REQUIRE(arr.size() == ref.size());
        REQUIRE(Traits::compare(arr.data(), ref.data(), ref.size()) == 0);
        // with count = 0
        arr.assign(data.hello_world, data.hello_world_len);
        arr.replace(0, 0, vec.cbegin(), vec.cend());
        ref.assign(12, l);
        ref += data.hello_world;
        REQUIRE(arr.size() == ref.size());
        REQUIRE(Traits::compare(arr.data(), ref.data(), ref.size()) == 0);

        // replace in the middle
        arr.assign(data.hello_world, data.hello_world_len);
        arr.replace(3, 6, vec.begin(), vec.begin() + 2);
        ref.assign(data.hello_world, 3);
        ref.append(2, l);
        ref += data.hello_world + 9;
        REQUIRE(arr.size() == ref.size());
        REQUIRE(Traits::compare(arr.data(), ref.data(), ref.size()) == 0);

        // replace at the end
        arr.assign(data.hello_world, data.hello_world_len);
        arr.replace(6, 6, vec.rbegin(), vec.rend());
        ref.assign(data.hello_world, 6);
        ref.append(12, l);
        REQUIRE(arr.size() == ref.size());
        REQUIRE(Traits::compare(arr.data(), ref.data(), ref.size()) == 0);
    }
}
TEST_CASE_TEMPLATE_APPLY(StorageArray_replace, CharTypes);

/* truncating when exceeding max_size() during replace() */
TEST_CASE_TEMPLATE_DEFINE("StorageArray replace truncation", CharType, StorageArray_replace_trunc)
{
    // this variant definitely won't throw
    using ArrayType = spsl::StorageArray<CharType, 64, spsl::policy::overflow::Truncate>;
    using Traits = typename ArrayType::traits_type;
    const TestData<CharType> data;

    const CharType ch = data.hello_world[0];
    const CharType ch2 = data.hello_world[1];
    ArrayType arr;

    arr.assign(arr.max_size() - 3, ch);
    REQUIRE(arr.size() == arr.max_size() - 3);
    // 3 more characters can fit
    REQUIRE_NOTHROW(arr.replace(0, 0, 3, ch));
    REQUIRE(arr.size() == arr.max_size());

    // any character more and it will be truncated
    // (1) replace with individual characters
    REQUIRE_NOTHROW(arr.replace(0, 1, 12, ch2));
    REQUIRE(arr.size() == arr.max_size());
    for (size_t i = 0; i < 12; ++i)
        REQUIRE(arr[i] == ch2);
    for (size_t i = 12; i < arr.size(); ++i)
        REQUIRE(arr[i] == ch);

    // (2) replace with a string
    REQUIRE_NOTHROW(arr.replace(0, 0, data.blablabla, data.blablabla_len));
    REQUIRE(Traits::compare(arr.data(), data.blablabla, data.blablabla_len) == 0);
    for (size_t i = data.blablabla_len; i < data.blablabla_len + 12; ++i)
        REQUIRE(arr[i] == ch2);
    for (size_t i = data.blablabla_len + 12; i < arr.size(); ++i)
        REQUIRE(arr[i] == ch);

    // (3) replace with an iterator range
    const std::basic_string<CharType> s(3, ch);
    REQUIRE_NOTHROW(arr.replace(0, 1, s.begin(), s.end()));
    for (size_t i = 0; i < 3; ++i)
        REQUIRE(arr[i] == ch);
    REQUIRE(Traits::compare(arr.data() + 3, data.blablabla + 1, data.blablabla_len - 1) == 0);
    for (size_t i = data.blablabla_len + 2; i < data.blablabla_len + 14; ++i)
        REQUIRE(arr[i] == ch2);
    for (size_t i = data.blablabla_len + 14; i < arr.size(); ++i)
        REQUIRE(arr[i] == ch);
}
TEST_CASE_TEMPLATE_APPLY(StorageArray_replace_trunc, CharTypes);

/* std::length_error when exceeding max_size() during replace() */
TEST_CASE_TEMPLATE_DEFINE("StorageArray replace length error", CharType,
                          StorageArray_replace_length_err)
{
    // this variant definitely will throw
    using ArrayType = spsl::StorageArray<CharType, 64, spsl::policy::overflow::Throw>;
    using Traits = typename ArrayType::traits_type;
    const TestData<CharType> data;

    const CharType ch = data.hello_world[0];
    const CharType ch2 = data.hello_world[1];
    ArrayType arr;

    arr.assign(arr.max_size() - 3, ch);
    REQUIRE(arr.size() == arr.max_size() - 3);
    // 3 more characters can fit
    REQUIRE_NOTHROW(arr.replace(0, 0, 3, ch));
    REQUIRE(arr.size() == arr.max_size());

    // any character more and std::length_error is thrown, but the string is unchanged
    const std::basic_string<CharType> ref(arr.data(), arr.size());
    // (1) replace with individual characters
    REQUIRE_THROWS_AS(arr.replace(0, 1, 12, ch2), std::length_error);
    REQUIRE(arr.size() == ref.size());
    REQUIRE(Traits::compare(arr.data(), ref.data(), ref.size()) == 0);

    // (2) replace with a string
    REQUIRE_THROWS_AS(arr.replace(0, data.blablabla_len - 1, data.blablabla, data.blablabla_len),
                      std::length_error);
    REQUIRE(arr.size() == ref.size());
    REQUIRE(Traits::compare(arr.data(), ref.data(), ref.size()) == 0);

    // (3) replace with an iterator range
    const std::basic_string<CharType> s(3, ch);
    REQUIRE_THROWS_AS(arr.replace(0, 1, s.begin(), s.end()), std::length_error);
    REQUIRE(arr.size() == ref.size());
    REQUIRE(Traits::compare(arr.data(), ref.data(), ref.size()) == 0);
}
TEST_CASE_TEMPLATE_APPLY(StorageArray_replace_length_err, CharTypes);

/* erase function */
TEST_CASE_TEMPLATE_DEFINE("StorageArray erase", CharType, StorageArray_erase)
{
    using ArrayType = spsl::StorageArray<CharType, 64>;
    using Traits = typename ArrayType::traits_type;
    const TestData<CharType> data;
    using RefType = std::basic_string<CharType>;

    // void erase(size_type index, size_type count)
    ArrayType arr;
    RefType ref;
    arr.assign(data.hello_world, data.hello_world_len);
    ref = data.hello_world;
    REQUIRE(arr.size() == ref.size());
    REQUIRE(Traits::compare(arr.data(), ref.data(), ref.size()) == 0);

    arr.erase(0, 1);
    ref.erase(0, 1);
    REQUIRE(arr.size() == ref.size());
    REQUIRE(Traits::compare(arr.data(), ref.data(), ref.size()) == 0);

    arr.erase(5, 3);
    ref.erase(5, 3);
    REQUIRE(arr.size() == ref.size());
    REQUIRE(Traits::compare(arr.data(), ref.data(), ref.size()) == 0);

    arr.erase(0, arr.size());
    ref.erase(0, ref.size());
    REQUIRE(arr.size() == ref.size());
    REQUIRE(Traits::compare(arr.data(), ref.data(), ref.size()) == 0);
    REQUIRE(arr.empty());
}
TEST_CASE_TEMPLATE_APPLY(StorageArray_erase, CharTypes);
