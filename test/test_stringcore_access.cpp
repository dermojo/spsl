/**
 * @author	Daniel Evers
 * @brief	StringBase unit tests: various accessor functions
 * @license MIT
 */

#include "catch.hpp"

#include "spsl.hpp"
#include "testdata.hpp"
#include "tests.hpp"


/* access functions */
TEMPLATE_LIST_TEST_CASE("StringBase access", "[string_core]", StringBaseTestTypes)
{
    using StringType = TestType;
    using StorageType = typename StringType::storage_type;
    using CharType = typename StorageType::char_type;
    using Traits = typename StringType::traits_type;
    using StringView = typename StringType::string_view_type;
    const TestData<CharType> data;
    const CharType nul = StorageType::nul;

    // 1. empty string, const
    {
        const StringType s;
        REQUIRE(s.c_str() != nullptr);
        REQUIRE(s.data() != nullptr);
        REQUIRE(s.c_str() == s.data());
        REQUIRE(Traits::compare(s.c_str(), data.empty, 1) == 0);
        REQUIRE(s[0] == nul);
        // at() will throw
        REQUIRE_THROWS_AS(s.at(0), std::out_of_range);
        REQUIRE_THROWS_AS(s.at(1), std::out_of_range);
        // front() and back() won't throw, but back() isn't allowed here
        REQUIRE(s.front() == nul);
        const StringView v = s;
        REQUIRE(v.data() == s.data());
        REQUIRE(v.size() == s.size());
        REQUIRE(v == s);
    }

    // 2. empty string, non-const
    {
        StringType s;
        REQUIRE(s.c_str() != nullptr);
        REQUIRE(s.data() != nullptr);
        REQUIRE(s.c_str() == s.data());
        REQUIRE(Traits::compare(s.c_str(), data.empty, 1) == 0);
        REQUIRE(s[0] == nul);
        // at() will throw
        REQUIRE_THROWS_AS(s.at(0), std::out_of_range);
        REQUIRE_THROWS_AS(s.at(1), std::out_of_range);
        // front() and back() won't throw, but back() isn't allowed here
        REQUIRE(s.front() == nul);
        const StringView v = s;
        REQUIRE(v.data() == s.data());
        REQUIRE(v.size() == s.size());
        REQUIRE(v == s);
    }

    // 3. non-empty string, const
    {
        const StringType s(data.hello_world);
        REQUIRE(s.c_str() != nullptr);
        REQUIRE(s.data() != nullptr);
        REQUIRE(s.c_str() == s.data());
        REQUIRE(Traits::compare(s.c_str(), data.hello_world, data.hello_world_len) == 0);
        REQUIRE(s.size() == data.hello_world_len);
        REQUIRE(s.length() == data.hello_world_len);
        // note: comparision includes NUL
        for (size_t i = 0; i <= s.size(); ++i)
            REQUIRE(s[i] == data.hello_world[i]);
        for (size_t i = 0; i < s.size(); ++i)
            REQUIRE(s.at(i) == data.hello_world[i]);
        // at() will throw if pos >= size()
        REQUIRE_THROWS_AS(s.at(data.hello_world_len), std::out_of_range);
        REQUIRE_THROWS_AS(s.at(data.hello_world_len + 47), std::out_of_range);

        REQUIRE(s.front() == data.hello_world[0]);
        REQUIRE(s.back() == data.hello_world[data.hello_world_len - 1]);
        const StringView v = s;
        REQUIRE(v.data() == s.data());
        REQUIRE(v.size() == s.size());
        REQUIRE(v == s);
    }

    // 4. non-empty string, non-const
    {
        StringType s(data.hello_world);
        REQUIRE(s.c_str() != nullptr);
        REQUIRE(s.data() != nullptr);
        REQUIRE(s.c_str() == s.data());
        REQUIRE(Traits::compare(s.c_str(), data.hello_world, data.hello_world_len) == 0);
        REQUIRE(s.size() == data.hello_world_len);
        REQUIRE(s.length() == data.hello_world_len);
        // note: comparision includes NUL
        for (size_t i = 0; i <= s.size(); ++i)
            REQUIRE(s[i] == data.hello_world[i]);
        for (size_t i = 0; i < s.size(); ++i)
            REQUIRE(s.at(i) == data.hello_world[i]);
        // at() will throw if pos >= size()
        REQUIRE_THROWS_AS(s.at(data.hello_world_len), std::out_of_range);
        REQUIRE_THROWS_AS(s.at(data.hello_world_len + 47), std::out_of_range);

        REQUIRE(s.front() == data.hello_world[0]);
        REQUIRE(s.back() == data.hello_world[data.hello_world_len - 1]);

        // here, data() is writable
        s.data()[0] = nul;
        REQUIRE_FALSE(Traits::compare(s.c_str(), data.hello_world, data.hello_world_len) == 0);
        REQUIRE(Traits::length(s.data()) == 0u);
        REQUIRE(Traits::length(s.c_str()) == 0u);
        const StringView v = s;
        REQUIRE(v.data() == s.data());
        REQUIRE(v.size() == s.size());
        REQUIRE(v == s);
    }
}

/* iterator functions */
TEMPLATE_LIST_TEST_CASE("StringBase iterator", "[string_core]", StringBaseTestTypes)
{
    using StringType = TestType;
    using StorageType = typename StringType::storage_type;
    using CharType = typename StorageType::char_type;
    const TestData<CharType> data;

    // 1. constant
    {
        const StringType s(data.hello_world);

        // begin() + end()
        size_t i = 0;
        for (auto it = s.begin(); it != s.end(); ++it)
            REQUIRE(*it == data.hello_world[i++]);

        // cbegin() + cend()
        i = 0;
        for (auto it = s.cbegin(); it != s.cend(); ++it)
            REQUIRE(*it == data.hello_world[i++]);

        // reverse tests - create a reference string for comparisons
        const std::basic_string<CharType> hw(data.hello_world);
        {
            // rbegin() + rend()
            auto it1 = s.rbegin();
            auto it2 = hw.rbegin();
            while (it1 != s.rend() && it2 != hw.rend())
            {
                REQUIRE(*it1 == *it2);
                ++it1;
                ++it2;
            }
            REQUIRE(it1 == s.rend());
            REQUIRE(it2 == hw.rend());
        }
        {
            // crbegin() + crend()
            auto it1 = s.crbegin();
            auto it2 = hw.crbegin();
            while (it1 != s.crend() && it2 != hw.crend())
            {
                REQUIRE(*it1 == *it2);
                ++it1;
                ++it2;
            }
            REQUIRE(it1 == s.crend());
            REQUIRE(it2 == hw.crend());
        }
    }

    // 2. non-constant
    {
        StringType s(data.hello_world);

        // begin() + end()
        size_t i = 0;
        for (auto it = s.begin(); it != s.end(); ++it)
            REQUIRE(*it == data.hello_world[i++]);

        // cbegin() + cend()
        i = 0;
        for (auto it = s.cbegin(); it != s.cend(); ++it)
            REQUIRE(*it == data.hello_world[i++]);

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
                REQUIRE(*it1 == *it2);
                ++it1;
                ++it2;
            }
            REQUIRE(it1 == s.rend());
            REQUIRE(it2 == hw.rend());
        }
        {
            // crbegin() + crend()
            auto it1 = s.crbegin();
            auto it2 = hw.crbegin();
            while (it1 != s.crend() && it2 != hw.crend())
            {
                REQUIRE(*it1 == *it2);
                ++it1;
                ++it2;
            }
            REQUIRE(it1 == s.crend());
            REQUIRE(it2 == hw.crend());
        }
    }
}


/* capacity functions */
TEMPLATE_LIST_TEST_CASE("StringBase capacity", "[string_core]", StringBaseTestTypes)
{
    using StringType = TestType;
    using StorageType = typename StringType::storage_type;
    using CharType = typename StorageType::char_type;
    const TestData<CharType> data;

    StringType s;
    REQUIRE(s.empty());
    REQUIRE(s.size() == 0u);
    REQUIRE(s.length() == 0u);
    REQUIRE(s.max_size() > 0u);
    REQUIRE(s.capacity() >= 0u);
    // note: max is 64
    s.reserve(57);
    REQUIRE(s.capacity() >= 57u);
    // no general test, only call it...
    s.shrink_to_fit();

    s = data.blablabla;
    REQUIRE_FALSE(s.empty());
    REQUIRE(s.size() == data.blablabla_len);
    REQUIRE(s.length() == data.blablabla_len);
    REQUIRE(s.max_size() >= data.blablabla_len);
    REQUIRE(s.capacity() >= data.blablabla_len);
    // note: max is 64
    REQUIRE(57u >= data.blablabla_len);
    s.reserve(57);
    REQUIRE(s.capacity() >= 57u);
    // no general test, only call it...
    s.shrink_to_fit();
    REQUIRE(s == data.blablabla);
}


/* operations */
TEMPLATE_LIST_TEST_CASE("StringBase operations", "[string_core]", StringBaseTestTypes)
{
    using StringType = TestType;
    using StorageType = typename StringType::storage_type;
    using CharType = typename StorageType::char_type;
    using Traits = typename StringType::traits_type;
    const TestData<CharType> data;
    const CharType nul = StorageType::nul;

    StringType s1;
    const CharType ch = data.hello_world[3];

    // clear
    s1 = data.hello_world;
    REQUIRE_FALSE(s1.empty());
    s1.clear();
    REQUIRE(s1.empty());

    // push_back
    const size_t num = 20;
    for (size_t i = 0; i < num; ++i)
        s1.push_back(ch);
    REQUIRE(s1.size() == num);
    for (size_t i = 0; i < num; ++i)
        REQUIRE(s1[i] == ch);
    REQUIRE(s1[num] == nul);

    // pop_back
    s1 = data.hello_world;
    REQUIRE_FALSE(s1.empty());
    size_t numPops = 0;
    while (!s1.empty())
    {
        s1.pop_back();
        ++numPops;
        REQUIRE(s1.size() == data.hello_world_len - numPops);
        REQUIRE(Traits::compare(s1.c_str(), data.hello_world, s1.length()) == 0);
        const std::basic_string<CharType> ref(data.hello_world, s1.length());
        REQUIRE(s1 == ref);
    }
    REQUIRE(numPops == data.hello_world_len);
    REQUIRE(s1.empty());

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
            REQUIRE(sub1 == subRef);
            REQUIRE(sub2 == subRef);
            REQUIRE(sub1.size() == len);
            REQUIRE(sub2.size() == len);
        }

        // again with length = npos
        {
            auto subRef = ref.substr(pos, std::basic_string<CharType>::npos);
            auto sub1 = s1.substr(pos, StringType::npos);
            auto sub2 = s2.substr(pos, StringType::npos);
            REQUIRE(sub1 == subRef);
            REQUIRE(sub2 == subRef);
            REQUIRE(sub1.size() == data.hello_world_len - pos);
            REQUIRE(sub2.size() == data.hello_world_len - pos);
        }
        // again without length (so it defaults to npos again)
        {
            auto subRef = ref.substr(pos);
            auto sub1 = s1.substr(pos);
            auto sub2 = s2.substr(pos);
            REQUIRE(sub1 == subRef);
            REQUIRE(sub2 == subRef);
            REQUIRE(sub1.size() == data.hello_world_len - pos);
            REQUIRE(sub2.size() == data.hello_world_len - pos);
        }
    }
    // substr throws if pos > size()
    REQUIRE_THROWS_AS(s1.substr(s1.size() + 1), std::out_of_range);
    REQUIRE_THROWS_AS(s2.substr(s2.size() + 1), std::out_of_range);

    // copy
    CharType buffer[256];
    for (size_t pos = 0; pos <= data.hello_world_len; ++pos)
    {
        // + 10 because we are allowed to ask for more
        for (size_t len = 0; len <= data.hello_world_len - pos + 10; ++len)
        {
            REQUIRE(s1 == data.hello_world);
            size_t n1 = s1.copy(buffer, len, pos);
            // unchanged
            REQUIRE(s1 == data.hello_world);

            // we can get only as much as there is left...
            REQUIRE(n1 == std::min(len, data.hello_world_len - pos));
            REQUIRE(Traits::compare(buffer, data.hello_world + pos, n1) == 0);
        }
    }
    // copy throws if pos > size()
    REQUIRE_THROWS_AS(s1.copy(buffer, 0, s1.size() + 1), std::out_of_range);
    REQUIRE_THROWS_AS(s2.copy(buffer, 0, s2.size() + 1), std::out_of_range);

    // resize
    REQUIRE(s1 == data.hello_world);
    REQUIRE(s1.size() == data.hello_world_len);
    // add a NUL character
    s1.append(1, s1.nul);
    REQUIRE(s1.size() == data.hello_world_len + 1);
    REQUIRE(Traits::compare(s1.c_str(), data.hello_world, data.hello_world_len) == 0);
    REQUIRE(s1[s1.size()] == nul);
    REQUIRE(s1[s1.size() - 1] == nul);
    // remove that character again
    s1.resize(data.hello_world_len);
    REQUIRE(s1 == data.hello_world);
    REQUIRE(s1.size() == data.hello_world_len);
    // shrink tests
    for (size_t i = data.hello_world_len; i != static_cast<size_t>(-1); --i)
    {
        s1.resize(i);
        REQUIRE(s1.size() == i);
        REQUIRE(s1.length() == i);
        REQUIRE(Traits::compare(s1.c_str(), data.hello_world, i) == 0);
    }
    REQUIRE(s1.empty());
    // grow tests
    for (size_t i = 1; i <= data.hello_world_len; ++i)
    {
        s1.resize(i, data.hello_world[i - 1]);
        REQUIRE(s1.size() == i);
        REQUIRE(s1.length() == i);
        REQUIRE(Traits::compare(s1.c_str(), data.hello_world, i) == 0);
    }

    // swap
    StringType s3(data.blablabla);
    s1.clear();
    REQUIRE(s1.empty());
    REQUIRE_FALSE(s3.empty());
    REQUIRE(s3 == data.blablabla);
    s1.swap(s3);
    REQUIRE(s3.empty());
    REQUIRE_FALSE(s1.empty());
    REQUIRE(s1 == data.blablabla);
    s3 = data.hello_world;
    s3.swap(s1);
    REQUIRE_FALSE(s1.empty());
    REQUIRE_FALSE(s3.empty());
    REQUIRE(s1 == data.hello_world);
    REQUIRE(s3 == data.blablabla);
}
