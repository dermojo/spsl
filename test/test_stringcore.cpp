/**
 * @file	Special Purpose Strings Library: test_stringcore.cpp
 * @author	Daniel Evers
 * @brief	StringCore unit tests
 * @license MIT
 */

#include <gtest/gtest.h>

#include "spsl.hpp"
#include "testdata.hpp"

template <typename CharType>
class StringCoreTest : public ::testing::Test
{
};

// all string types we want to test: we already include StringBase here to avoid copy & paste
using SpecificTypes = testing::Types<
  spsl::StringCore<spsl::StorageArray<char, 64, spsl::policy::overflow::Truncate>>,
  spsl::StringCore<spsl::StorageArray<char, 64, spsl::policy::overflow::Throw>>,
  spsl::StringCore<spsl::StorageArray<wchar_t, 64, spsl::policy::overflow::Truncate>>,
  spsl::StringCore<spsl::StorageArray<wchar_t, 64, spsl::policy::overflow::Throw>>,
  spsl::StringCore<spsl::StoragePassword<char>>, spsl::StringCore<spsl::StoragePassword<wchar_t>>,
  spsl::StringCore<spsl::StorageArray<char, 64, spsl::policy::overflow::Truncate>>,
  spsl::StringBase<spsl::StorageArray<char, 64, spsl::policy::overflow::Throw>>,
  spsl::StringBase<spsl::StorageArray<wchar_t, 64, spsl::policy::overflow::Truncate>>,
  spsl::StringBase<spsl::StorageArray<wchar_t, 64, spsl::policy::overflow::Throw>>,
  spsl::StringBase<spsl::StoragePassword<char>>, spsl::StringBase<spsl::StoragePassword<wchar_t>>,
  spsl::ArrayString<128, spsl::policy::overflow::Truncate>,
  spsl::ArrayStringW<128, spsl::policy::overflow::Truncate>,
  spsl::ArrayString<128, spsl::policy::overflow::Throw>,
  spsl::ArrayStringW<128, spsl::policy::overflow::Throw>, spsl::PasswordString,
  spsl::PasswordStringW>;


TYPED_TEST_CASE(StringCoreTest, SpecificTypes);


/* check constructor availability */
TYPED_TEST(StringCoreTest, Constructors)
{
    using StringType = TypeParam; // gtest specific
    using StorageType = typename StringType::storage_type;
    using CharType = typename StorageType::char_type;
    using Traits = typename StringType::traits_type;
    const TestData<CharType> data{};

    // default constructor
    {
        const StringType s;
        ASSERT_EQ(s.length(), 0);
        ASSERT_EQ(s.size(), 0);
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
        ASSERT_EQ(s.length(), 5);
        ASSERT_EQ(s.size(), 5);
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
    const TestData<CharType> data{};

    // construct from input iterators
    {
        // 1: from a std::basic_string
        {
            std::basic_string<CharType> ref;
            ref = data.hello_world;
            StringType s(ref.begin(), ref.end());
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
            ASSERT_TRUE(Traits::compare(s.c_str(), data.hello_world, data.hello_world_len) == 0);
            ASSERT_TRUE(s == data.hello_world);
        }
    }
}

/* assignment operators */
TYPED_TEST(StringCoreTest, AssignmentOperators)
{
    using StringType = TypeParam; // gtest specific
    using StorageType = typename StringType::storage_type;
    using CharType = typename StorageType::char_type;
    using Traits = typename StringType::traits_type;
    const TestData<CharType> data{};
    std::initializer_list<CharType> initList = data.initializerList();

    StringType s;

    // const char_type*
    s = data.hello_world;
    ASSERT_EQ(s.size(), data.hello_world_len);
    ASSERT_TRUE(Traits::compare(s.c_str(), data.hello_world, data.hello_world_len) == 0);
    ASSERT_TRUE(s == data.hello_world);
    s = data.blablabla;
    ASSERT_EQ(s.size(), data.blablabla_len);
    ASSERT_TRUE(Traits::compare(s.c_str(), data.blablabla, data.blablabla_len) == 0);
    ASSERT_TRUE(s == data.blablabla);

    // char_type
    const CharType ch = data.hello_world[0];
    s = ch;
    ASSERT_EQ(s.size(), 1);
    ASSERT_TRUE(Traits::compare(s.c_str(), &ch, 1) == 0);
    ASSERT_TRUE(s[0] == ch);

    // initializer list
    s = initList;
    ASSERT_EQ(s.size(), initList.size());
    ASSERT_TRUE(Traits::compare(s.c_str(), initList.begin(), initList.size()) == 0);

    // copy
    StringType s2(data.blablabla);
    s = s2;
    ASSERT_EQ(s.size(), data.blablabla_len);
    ASSERT_TRUE(Traits::compare(s.c_str(), data.blablabla, data.blablabla_len) == 0);
    ASSERT_TRUE(s == data.blablabla);

    // move
    s2 = data.hello_world;
    s = std::move(s2);
    ASSERT_EQ(s.size(), data.hello_world_len);
    ASSERT_TRUE(Traits::compare(s.c_str(), data.hello_world, data.hello_world_len) == 0);
    ASSERT_TRUE(s == data.hello_world);

    // other string classes
    // 1. std::basic_string
    std::basic_string<CharType> str(data.blablabla);
    s = str;
    ASSERT_EQ(s.size(), data.blablabla_len);
    ASSERT_TRUE(Traits::compare(s.c_str(), data.blablabla, data.blablabla_len) == 0);
    ASSERT_TRUE(s == data.blablabla);
    // 2. std::vector
    std::vector<CharType> vec;
    vec.resize(data.hello_world_len);
    Traits::copy(vec.data(), data.hello_world, data.hello_world_len);
    s = vec;
    ASSERT_EQ(s.size(), data.hello_world_len);
    ASSERT_TRUE(Traits::compare(s.c_str(), data.hello_world, data.hello_world_len) == 0);
    ASSERT_TRUE(s == data.hello_world);
// 3. basic_string_view if available
#ifdef TEST_STRING_VIEW
    std::experimental::basic_string_view<CharType> view(data.blablabla, data.blablabla_len);
    s = view;
    ASSERT_EQ(s.size(), data.blablabla_len);
    ASSERT_TRUE(Traits::compare(s.c_str(), data.blablabla, data.blablabla_len) == 0);
    ASSERT_TRUE(s == data.blablabla);
#endif
}


/* assignment functions */
TYPED_TEST(StringCoreTest, AssignmentFunctions)
{
    using StringType = TypeParam; // gtest specific
    using StorageType = typename StringType::storage_type;
    using CharType = typename StorageType::char_type;
    using Traits = typename StringType::traits_type;
    const TestData<CharType> data{};
    std::initializer_list<CharType> initList = data.initializerList();
    const CharType nul = StorageType::nul;

    StringType s;
    const CharType ch = data.hello_world[0];

    // count + character
    s.assign(42, ch);
    ASSERT_EQ(s.size(), 42);
    for (auto c : s)
        ASSERT_EQ(c, ch);
    for (size_t i = 0; i < s.size(); ++i)
        ASSERT_EQ(s[i], ch);
    ASSERT_EQ(s[s.size()], nul);

    // const char_type* + length
    s.assign(data.blablabla, data.blablabla_len);
    ASSERT_EQ(s.size(), data.blablabla_len);
    ASSERT_TRUE(Traits::compare(s.c_str(), data.blablabla, data.blablabla_len) == 0);
    ASSERT_TRUE(s == data.blablabla);

    // const char_type*
    s.assign(data.hello_world);
    ASSERT_EQ(s.size(), data.hello_world_len);
    ASSERT_TRUE(Traits::compare(s.c_str(), data.hello_world, data.hello_world_len) == 0);
    ASSERT_TRUE(s == data.hello_world);
    s.assign(data.blablabla);
    ASSERT_EQ(s.size(), data.blablabla_len);
    ASSERT_TRUE(Traits::compare(s.c_str(), data.blablabla, data.blablabla_len) == 0);
    ASSERT_TRUE(s == data.blablabla);

    // initializer list
    s.assign(initList);
    ASSERT_EQ(s.size(), initList.size());
    ASSERT_TRUE(Traits::compare(s.c_str(), initList.begin(), initList.size()) == 0);

    // iterators
    const std::basic_string<CharType> str(data.blablabla);
    s.assign(str.begin(), str.end());
    ASSERT_EQ(s.size(), data.blablabla_len);
    ASSERT_TRUE(Traits::compare(s.c_str(), data.blablabla, data.blablabla_len) == 0);
    ASSERT_TRUE(s == data.blablabla);

    // copy
    StringType s2(data.blablabla);
    s.assign(s2);
    ASSERT_EQ(s.size(), data.blablabla_len);
    ASSERT_TRUE(Traits::compare(s.c_str(), data.blablabla, data.blablabla_len) == 0);
    ASSERT_TRUE(s == data.blablabla);

    // move
    s2 = data.hello_world;
    s.assign(std::move(s2));
    ASSERT_EQ(s.size(), data.hello_world_len);
    ASSERT_TRUE(Traits::compare(s.c_str(), data.hello_world, data.hello_world_len) == 0);
    ASSERT_TRUE(s == data.hello_world);

    // other string classes
    // 1. std::basic_string (reuse the object from above)
    s.assign(str);
    ASSERT_EQ(s.size(), data.blablabla_len);
    ASSERT_TRUE(Traits::compare(s.c_str(), data.blablabla, data.blablabla_len) == 0);
    ASSERT_TRUE(s == data.blablabla);
    // 2. std::vector
    std::vector<CharType> vec;
    vec.resize(data.hello_world_len);
    Traits::copy(vec.data(), data.hello_world, data.hello_world_len);
    s.assign(vec);
    ASSERT_EQ(s.size(), data.hello_world_len);
    ASSERT_TRUE(Traits::compare(s.c_str(), data.hello_world, data.hello_world_len) == 0);
    ASSERT_TRUE(s == data.hello_world);
// 3. basic_string_view if available
#ifdef TEST_STRING_VIEW
    std::experimental::basic_string_view<CharType> view(data.blablabla, data.blablabla_len);
    s.assign(view);
    ASSERT_EQ(s.size(), data.blablabla_len);
    ASSERT_TRUE(Traits::compare(s.c_str(), data.blablabla, data.blablabla_len) == 0);
    ASSERT_TRUE(s == data.blablabla);
#endif

    // other string class with pos and count
    s.assign(str, 1, 3);
    const auto sub = str.substr(1, 3);
    ASSERT_EQ(s.size(), sub.size());
    ASSERT_TRUE(Traits::compare(s.c_str(), sub.data(), sub.size()) == 0);
    ASSERT_TRUE(s == sub);
}


/* access functions */
TYPED_TEST(StringCoreTest, AccessFunctions)
{
    using StringType = TypeParam; // gtest specific
    using StorageType = typename StringType::storage_type;
    using CharType = typename StorageType::char_type;
    using Traits = typename StringType::traits_type;
    const TestData<CharType> data{};
    const CharType nul = StorageType::nul;

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

        // here, data() is writeable
        s.data()[0] = nul;
        ASSERT_FALSE(Traits::compare(s.c_str(), data.hello_world, data.hello_world_len) == 0);
        ASSERT_EQ(Traits::length(s.data()), 0);
        ASSERT_EQ(Traits::length(s.c_str()), 0);
    }
}

/* iterator functions */
TYPED_TEST(StringCoreTest, IteratorFunctions)
{
    using StringType = TypeParam; // gtest specific
    using StorageType = typename StringType::storage_type;
    using CharType = typename StorageType::char_type;
    const TestData<CharType> data{};

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
    const TestData<CharType> data{};

    StringType s;
    ASSERT_TRUE(s.empty());
    ASSERT_EQ(s.size(), 0);
    ASSERT_EQ(s.length(), 0);
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
    const TestData<CharType> data{};
    const CharType nul = StorageType::nul;

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

/* append functions */
TYPED_TEST(StringCoreTest, AppendFunctions)
{
    using StringType = TypeParam; // gtest specific
    using StorageType = typename StringType::storage_type;
    using CharType = typename StorageType::char_type;
    using Traits = typename StringType::traits_type;
    const TestData<CharType> data{};
    std::initializer_list<CharType> initList = data.initializerList();
    const CharType ch = data.hello_world[5];

    // we perform all operations simultaneously on both string types for easier comparison
    StringType s1;
    std::basic_string<CharType> s2;

    // count, char
    s1.append(17, ch);
    s2.append(17, ch);
    ASSERT_TRUE(s1 == s2);
    ASSERT_EQ(s1.size(), s2.size());
    ASSERT_TRUE(Traits::compare(s1.c_str(), s2.c_str(), s1.size()) == 0);

    // char_type*, count
    s1.append(data.blablabla, data.blablabla_len);
    s2.append(data.blablabla, data.blablabla_len);
    ASSERT_TRUE(s1 == s2);
    ASSERT_EQ(s1.size(), s2.size());
    ASSERT_TRUE(Traits::compare(s1.c_str(), s2.c_str(), s1.size()) == 0);

    // char_type*
    s1.append(data.hello_world);
    s2.append(data.hello_world);
    ASSERT_TRUE(s1 == s2);
    ASSERT_EQ(s1.size(), s2.size());
    ASSERT_TRUE(Traits::compare(s1.c_str(), s2.c_str(), s1.size()) == 0);

    s1.clear();
    s2.clear();

    // initializer list
    s1.append(initList);
    s2.append(initList);
    ASSERT_TRUE(s1 == s2);
    ASSERT_EQ(s1.size(), s2.size());
    ASSERT_TRUE(Traits::compare(s1.c_str(), s2.c_str(), s1.size()) == 0);

    // iterator range
    const std::vector<CharType> vec(23, ch);
    s1.append(vec);
    // std::basic_string can' do this
    s2.append(23, ch);
    ASSERT_TRUE(s1 == s2);
    ASSERT_EQ(s1.size(), s2.size());
    ASSERT_TRUE(Traits::compare(s1.c_str(), s2.c_str(), s1.size()) == 0);

    // other string class
    const std::basic_string<CharType> other(data.blablabla);
    s1.append(other);
    s2.append(other);
    ASSERT_TRUE(s1 == s2);
    ASSERT_EQ(s1.size(), s2.size());
    ASSERT_TRUE(Traits::compare(s1.c_str(), s2.c_str(), s1.size()) == 0);

    s1.clear();
    s2.clear();

    // other string class with pos and count
    s1.append(other, 2, other.size() - 3);
    s2.append(other, 2, other.size() - 3);
    ASSERT_TRUE(s1 == s2);
    ASSERT_EQ(s1.size(), s2.size());
    ASSERT_TRUE(Traits::compare(s1.c_str(), s2.c_str(), s1.size()) == 0);

    // += char_type
    s1 += ch;
    s2 += ch;
    ASSERT_TRUE(s1 == s2);
    ASSERT_EQ(s1.size(), s2.size());
    ASSERT_TRUE(Traits::compare(s1.c_str(), s2.c_str(), s1.size()) == 0);

    // += char_type*
    s1 += data.hello_world;
    s2 += data.hello_world;
    ASSERT_TRUE(s1 == s2);
    ASSERT_EQ(s1.size(), s2.size());
    ASSERT_TRUE(Traits::compare(s1.c_str(), s2.c_str(), s1.size()) == 0);

    // += initializer_list
    s1 += initList;
    s2 += initList;
    ASSERT_TRUE(s1 == s2);
    ASSERT_EQ(s1.size(), s2.size());
    ASSERT_TRUE(Traits::compare(s1.c_str(), s2.c_str(), s1.size()) == 0);

    s1.clear();
    s2.clear();

    // += other string class
    s1 += other;
    s2 += other;
    ASSERT_TRUE(s1 == s2);
    ASSERT_EQ(s1.size(), s2.size());
    ASSERT_TRUE(Traits::compare(s1.c_str(), s2.c_str(), s1.size()) == 0);
}

/* comparison functions */
TYPED_TEST(StringCoreTest, ComparisonFunctions)
{
    using StringType = TypeParam; // gtest specific
    using StorageType = typename StringType::storage_type;
    using CharType = typename StorageType::char_type;
    const TestData<CharType> data{};

    const StringType s(data.hello_world);
    StringType ref(data.hello_world);
    // note: if the prefix is identical, comparing with a shorter string yields rc > 0

    // char_type*
    ASSERT_EQ(s.compare(ref.c_str()), 0);
    ref[0]++;
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
    ref[0]++;
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
    const TestData<CharType> data{};
    const auto npos = StringType::npos;

    const StringType s(data.hello_world);
    const CharType H = data.hello_world[0];
    const CharType W = data.hello_world[6];
    const CharType b = data.blablabla[0];
    const CharType* world = data.hello_world + 6;
    const auto world_len = Traits::length(world);

    // find: char_type*, pos, count
    ASSERT_EQ(s.find(s.data(), 0, s.size()), 0);
    ASSERT_EQ(s.find(world, 0, world_len), 6);
    ASSERT_EQ(s.find(world, 0, 1), 6);
    ASSERT_EQ(s.find(world, 7, world_len), npos);

    // find: char_type, pos
    ASSERT_EQ(s.find(b, 0), npos);
    ASSERT_EQ(s.find(W), 6);
    ASSERT_EQ(s.find(H), 0);
    ASSERT_EQ(s.find(H, 1), npos);

    // find: char_type*, pos
    ASSERT_EQ(s.find(s.data()), 0);
    ASSERT_EQ(s.find(s.data(), 0), 0);
    ASSERT_EQ(s.find(world), 6);
    ASSERT_EQ(s.find(world, 0), 6);
    ASSERT_EQ(s.find(world, 7), npos);

    // find: String, pos
    const std::basic_string<CharType> ref(world);
    ASSERT_EQ(s.find(s), 0);
    ASSERT_EQ(s.find(s, 0), 0);
    ASSERT_EQ(s.find(ref), 6);
    ASSERT_EQ(s.find(ref, 0), 6);
    ASSERT_EQ(s.find(ref, 7), npos);

    // rfind: char_type*, pos, count
    ASSERT_EQ(s.rfind(s.data(), s.size() - 1, s.size()), 0);
    ASSERT_EQ(s.rfind(s.data(), npos, s.size()), 0);
    ASSERT_EQ(s.rfind(world, npos, world_len), 6);
    ASSERT_EQ(s.rfind(world, npos, 1), 6);
    ASSERT_EQ(s.rfind(world, 5, world_len), npos);

    // rfind: char_type, pos
    ASSERT_EQ(s.rfind(b, npos), npos);
    ASSERT_EQ(s.rfind(W), 6);
    ASSERT_EQ(s.rfind(H), 0);
    ASSERT_EQ(s.rfind(W, 5), npos);

    // rfind: char_type*, pos
    ASSERT_EQ(s.rfind(s.data()), 0);
    ASSERT_EQ(s.rfind(s.data(), s.size() - 1), 0);
    ASSERT_EQ(s.rfind(s.data(), npos), 0);
    ASSERT_EQ(s.rfind(world), 6);
    ASSERT_EQ(s.rfind(world, npos), 6);
    ASSERT_EQ(s.rfind(world, 5), npos);

    // rfind: String, pos
    ASSERT_EQ(s.rfind(s), 0);
    ASSERT_EQ(s.rfind(s, s.size() - 1), 0);
    ASSERT_EQ(s.rfind(s, npos), 0);
    ASSERT_EQ(s.rfind(ref), 6);
    ASSERT_EQ(s.rfind(ref, s.size() - 1), 6);
    ASSERT_EQ(s.rfind(ref, npos), 6);
    ASSERT_EQ(s.rfind(ref, 5), npos);
}

/* comparison operators */
TYPED_TEST(StringCoreTest, ComparisonOperators)
{
    using StringType = TypeParam; // gtest specific
    using StorageType = typename StringType::storage_type;
    using CharType = typename StorageType::char_type;
    const TestData<CharType> data{};

    const StringType s(data.hello_world);
    const StringType bla(data.blablabla);
    const StringType hello(data.hello_world);
    std::basic_string<CharType> ref;

    // 1. StringCore, char_type*
    ASSERT_TRUE(s == data.hello_world);
    ASSERT_TRUE(s != data.blablabla);
    ASSERT_FALSE(s == data.blablabla);
    ASSERT_TRUE(s <= data.hello_world);
    ASSERT_TRUE(s >= data.hello_world);
    ASSERT_FALSE(s < data.hello_world);
    ASSERT_FALSE(s > data.hello_world);
    ref = data.hello_world;
    ref[0]--;
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
    ref[0]--;
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
    ref2[0]--;
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
    ref[0]--;
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
    ref[0]--;
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


/* swap specialization */
TYPED_TEST(StringCoreTest, SwapSpecialization)
{
    using StringType = TypeParam; // gtest specific
    using StorageType = typename StringType::storage_type;
    using CharType = typename StorageType::char_type;
    const TestData<CharType> data{};

    StringType s1;

    // swap
    StringType s2(data.blablabla);
    ASSERT_TRUE(s1.empty());
    ASSERT_FALSE(s2.empty());
    ASSERT_TRUE(s2 == data.blablabla);
    std::swap(s1, s2);
    ASSERT_TRUE(s2.empty());
    ASSERT_FALSE(s1.empty());
    ASSERT_TRUE(s1 == data.blablabla);
    s2 = data.hello_world;
    std::swap(s2, s1);
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
    const TestData<CharType> data{};

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
