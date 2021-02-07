/**
 * @author	Daniel Evers
 * @brief	StringBase unit tests: various functions
 * @license MIT
 */

#include "catch.hpp"

#include "spsl.hpp"
#include "testdata.hpp"
#include "tests.hpp"

/* comparison functions */
TEMPLATE_LIST_TEST_CASE("StringBase comparison", "[string_core]", StringBaseTestTypes)
{
    using StringType = TestType;
    using StorageType = typename StringType::storage_type;
    using CharType = typename StorageType::char_type;
    const TestData<CharType> data;

    const StringType s(data.hello_world);
    StringType ref(data.hello_world);
    // note: if the prefix is identical, comparing with a shorter string yields rc > 0

    // char_type*
    REQUIRE(s.compare(ref.c_str()) == 0);
    ++ref[0];
    REQUIRE(s.compare(ref.c_str()) < 0);
    ref[0] = static_cast<CharType>(ref[0] - 2);
    REQUIRE(s.compare(ref.c_str()) > 0);
    ref = s.c_str();
    ref.pop_back();
    REQUIRE(s.compare(ref.c_str()) > 0);
    ref = s.c_str();
    ref += s[0];
    REQUIRE(s.compare(ref.c_str()) < 0);

    // pos, count, char_type*
    ref = s.c_str();
    REQUIRE(s.compare(0, s.size(), ref.c_str()) == 0);
    REQUIRE(s.compare(0, s.size() - 1, ref.c_str()) < 0);
    if (s[0] > s[1])
    {
        REQUIRE(s.compare(1, s.size(), ref.c_str()) < 0);
    }
    else if (s[0] < s[1])
    {
        REQUIRE(s.compare(1, s.size(), ref.c_str()) > 0);
    }
    REQUIRE(s.compare(1, s.size() - 1, ref.c_str() + 1) == 0);
    ref.pop_back();
    REQUIRE(s.compare(0, s.size(), ref.c_str()) > 0);

    // pos, count, char_type*, count
    ref = s.c_str();
    REQUIRE(s.compare(0, s.size(), ref.c_str(), ref.size()) == 0);
    REQUIRE(s.compare(0, s.size() - 3, ref.c_str(), ref.size() - 3) == 0);
    REQUIRE(s.compare(0, s.size() - 3, ref.c_str(), ref.size() - 2) < 0);
    REQUIRE(s.compare(0, s.size() - 2, ref.c_str(), ref.size() - 3) > 0);
    REQUIRE(s.compare(0, s.size() - 1, ref.c_str(), ref.size()) < 0);
    if (s[0] > s[1])
    {
        REQUIRE(s.compare(1, s.size(), ref.c_str(), ref.size()) < 0);
    }
    else if (s[0] < s[1])
    {
        REQUIRE(s.compare(1, s.size(), ref.c_str(), ref.size()) > 0);
    }
    REQUIRE(s.compare(1, s.size() - 1, ref.c_str() + 1, ref.size() - 1) == 0);
    ref.pop_back();
    REQUIRE(s.compare(0, s.size(), ref.c_str(), ref.size()) > 0);

    // other string class
    ref = s.c_str();
    REQUIRE(s.compare(ref) == 0);
    ++ref[0];
    ;
    REQUIRE(s.compare(ref) < 0);
    ref[0] = static_cast<CharType>(ref[0] - 2);
    REQUIRE(s.compare(ref) > 0);
    ref = s;
    ref.pop_back();
    REQUIRE(s.compare(ref) > 0);
    ref = s;
    ref += s[0];
    REQUIRE(s.compare(ref) < 0);

    // pos, count, string
    ref = s.c_str();
    REQUIRE(s.compare(0, s.size(), ref) == 0);
    REQUIRE(s.compare(0, s.size() - 1, ref) < 0);
    if (s[0] > s[1])
    {
        REQUIRE(s.compare(1, s.size(), ref) < 0);
    }
    else if (s[0] < s[1])
    {
        REQUIRE(s.compare(1, s.size(), ref) > 0);
    }
    auto ref2 = ref.substr(1);
    REQUIRE(s.compare(1, s.size() - 1, ref2) == 0);
    ref.pop_back();
    REQUIRE(s.compare(0, s.size(), ref) > 0);

    // pos, count, string, pos, count
    ref = s.c_str();
    REQUIRE(s.compare(0, s.size(), ref, 0, ref.size()) == 0);
    REQUIRE(s.compare(5, s.size() - 5, ref, 5, ref.size() - 5) == 0);
    REQUIRE(s.compare(0, s.size() - 3, ref, 0, ref.size() - 3) == 0);
    REQUIRE(s.compare(0, s.size() - 3, ref, 0, ref.size() - 2) < 0);
    REQUIRE(s.compare(0, s.size() - 2, ref, 0, ref.size() - 3) > 0);
    REQUIRE(s.compare(0, s.size() - 1, ref, 0, ref.size()) < 0);
    if (s[0] > s[1])
    {
        REQUIRE(s.compare(1, s.size(), ref, 0, ref.size()) < 0);
        REQUIRE(s.compare(0, s.size(), ref, 1, ref.size()) > 0);
    }
    else if (s[0] < s[1])
    {
        REQUIRE(s.compare(1, s.size(), ref, 0, ref.size()) > 0);
        REQUIRE(s.compare(0, s.size(), ref, 1, ref.size()) < 0);
    }
    REQUIRE(s.compare(1, s.size() - 1, ref, 1, ref.size() - 1) == 0);
    REQUIRE(s.compare(0, s.size(), ref, 0, ref.size() - 1) > 0);
}

/* find functions */
TEMPLATE_LIST_TEST_CASE("StringBase find", "[string_core]", StringBaseTestTypes)
{
    using StringType = TestType;
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
    REQUIRE(s.find(s.data(), 0, s.size()) == 0u);
    REQUIRE(s.find(world, 0, world_len) == 6u);
    REQUIRE(s.find(world, 0, 1) == 6u);
    REQUIRE(s.find(world, 7, world_len) == npos);
    // searching for an empty string finds the current position (if valid)
    REQUIRE(s.find(world, 0, 0) == 0u);
    REQUIRE(s.find(world, 3, 0) == 3u);
    REQUIRE(s.find(world, s.size(), 0) == s.size());
    REQUIRE(s.find(world, s.size() + 1, 0) == npos);

    // find: char_type, pos
    REQUIRE(s.find(b, 0) == npos);
    REQUIRE(s.find(W) == 6u);
    REQUIRE(s.find(H) == 0u);
    REQUIRE(s.find(H, 1) == npos);

    // find: char_type*, pos
    REQUIRE(s.find(s.data()) == 0u);
    REQUIRE(s.find(s.data(), 0) == 0u);
    REQUIRE(s.find(world) == 6u);
    REQUIRE(s.find(world, 0) == 6u);
    REQUIRE(s.find(world, 7) == npos);
    // searching for an empty string finds the current position (if valid)
    {
        const CharType empty[1] = { nul };
        REQUIRE(s.find(empty, 0, 0) == 0u);
        REQUIRE(s.find(empty, 3, 0) == 3u);
        REQUIRE(s.find(empty, s.size(), 0) == s.size());
        REQUIRE(s.find(empty, s.size() + 1, 0) == npos);
    }

    // find: String, pos
    const std::basic_string<CharType> ref(world);
    REQUIRE(s.find(s) == 0u);
    REQUIRE(s.find(s, 0) == 0u);
    REQUIRE(s.find(ref) == 6u);
    REQUIRE(s.find(ref, 0) == 6u);
    REQUIRE(s.find(ref, 7) == npos);
    // searching for an empty string finds the current position (if valid)
    {
        const std::basic_string<CharType> empty;
        REQUIRE(s.find(empty) == 0u);
        REQUIRE(s.find(empty, 3) == 3u);
        REQUIRE(s.find(empty, s.size()) == s.size());
        REQUIRE(s.find(empty, s.size() + 1) == npos);
    }

    // rfind: char_type*, pos, count
    REQUIRE(s.rfind(s.data(), s.size() - 1, s.size()) == 0u);
    REQUIRE(s.rfind(s.data(), npos, s.size()) == 0u);
    REQUIRE(s.rfind(world, npos, world_len) == 6u);
    REQUIRE(s.rfind(world, npos, 1) == 6u);
    REQUIRE(s.rfind(world, 5, world_len) == npos);

    // rfind: char_type, pos
    REQUIRE(s.rfind(b, npos) == npos);
    REQUIRE(s.rfind(W) == 6u);
    REQUIRE(s.rfind(H) == 0u);
    REQUIRE(s.rfind(W, 5) == npos);

    // rfind: char_type*, pos
    REQUIRE(s.rfind(s.data()) == 0u);
    REQUIRE(s.rfind(s.data(), s.size() - 1) == 0u);
    REQUIRE(s.rfind(s.data(), npos) == 0u);
    REQUIRE(s.rfind(world) == 6u);
    REQUIRE(s.rfind(world, npos) == 6u);
    REQUIRE(s.rfind(world, 5) == npos);

    // rfind: String, pos
    REQUIRE(s.rfind(s) == 0u);
    REQUIRE(s.rfind(s, s.size() - 1) == 0u);
    REQUIRE(s.rfind(s, npos) == 0u);
    REQUIRE(s.rfind(ref) == 6u);
    REQUIRE(s.rfind(ref, s.size() - 1) == 6u);
    REQUIRE(s.rfind(ref, npos) == 6u);
    REQUIRE(s.rfind(ref, 5) == npos);
}

/* comparison operators */
TEMPLATE_LIST_TEST_CASE("StringBase comparison operators", "[string_core]", StringBaseTestTypes)
{
    using StringType = TestType;
    using StorageType = typename StringType::storage_type;
    using CharType = typename StorageType::char_type;
    const TestData<CharType> data;

    const StringType s(data.hello_world);
    const StringType bla(data.blablabla);
    const StringType hello(data.hello_world);
    std::basic_string<CharType> ref;

    // 1. StringBase, char_type*
    REQUIRE(s == data.hello_world);
    REQUIRE(s != data.blablabla);
    REQUIRE_FALSE(s == data.blablabla);
    REQUIRE(s <= data.hello_world);
    REQUIRE(s >= data.hello_world);
    REQUIRE_FALSE(s < data.hello_world);
    REQUIRE_FALSE(s > data.hello_world);
    ref = data.hello_world;
    --ref[0];
    REQUIRE(s > ref.c_str());
    REQUIRE(s >= ref.c_str());
    REQUIRE_FALSE(s < ref.c_str());
    REQUIRE_FALSE(s <= ref.c_str());
    ref[0] = static_cast<CharType>(ref[0] + 2);
    REQUIRE(s < ref.c_str());
    REQUIRE(s <= ref.c_str());
    REQUIRE_FALSE(s > ref.c_str());
    REQUIRE_FALSE(s >= ref.c_str());

    // 2. char_type*, StringBase
    REQUIRE(data.hello_world == s);
    REQUIRE(data.blablabla != s);
    REQUIRE_FALSE(data.blablabla == s);
    REQUIRE(data.hello_world >= s);
    REQUIRE(data.hello_world <= s);
    REQUIRE_FALSE(data.hello_world > s);
    REQUIRE_FALSE(data.hello_world < s);
    ref = data.hello_world;
    --ref[0];
    REQUIRE(ref.c_str() < s);
    REQUIRE(ref.c_str() <= s);
    REQUIRE_FALSE(ref.c_str() > s);
    REQUIRE_FALSE(ref.c_str() >= s);
    ref[0] = static_cast<CharType>(ref[0] + 2);
    REQUIRE(ref.c_str() > s);
    REQUIRE(ref.c_str() >= s);
    REQUIRE_FALSE(ref.c_str() < s);
    REQUIRE_FALSE(ref.c_str() <= s);

    // 3. StringBase, StringBase
    REQUIRE(s == s);
    REQUIRE_FALSE(s != s);
    REQUIRE(s == hello);
    REQUIRE_FALSE(s != hello);
    REQUIRE_FALSE(s == bla);
    REQUIRE(s != bla);
    REQUIRE(s <= hello);
    REQUIRE(s >= hello);
    REQUIRE_FALSE(s < hello);
    REQUIRE_FALSE(s > hello);
    StringType ref2 = data.hello_world;
    --ref2[0];
    REQUIRE(s > ref2);
    REQUIRE(s >= ref2);
    REQUIRE_FALSE(s < ref2);
    REQUIRE_FALSE(s <= ref2);
    ref2[0] = static_cast<CharType>(ref2[0] + 2);
    REQUIRE(s < ref2);
    REQUIRE(s <= ref2);
    REQUIRE_FALSE(s > ref2);
    REQUIRE_FALSE(s >= ref2);

    // 4. StringBase, other string class
    ref = s.c_str();
    REQUIRE(s == ref);
    REQUIRE_FALSE(s != ref);
    REQUIRE(s == ref);
    REQUIRE_FALSE(s != ref);
    ref = data.blablabla;
    REQUIRE_FALSE(s == ref);
    REQUIRE(s != ref);
    ref = s.c_str();
    REQUIRE(s <= ref);
    REQUIRE(s >= ref);
    REQUIRE_FALSE(s < ref);
    REQUIRE_FALSE(s > ref);
    --ref[0];
    REQUIRE(s > ref);
    REQUIRE(s >= ref);
    REQUIRE_FALSE(s < ref);
    REQUIRE_FALSE(s <= ref);
    ref[0] = static_cast<CharType>(ref[0] + 2);
    REQUIRE(s < ref);
    REQUIRE(s <= ref);
    REQUIRE_FALSE(s > ref);
    REQUIRE_FALSE(s >= ref);

    // 5. other string class, StringBase
    ref = s.c_str();
    REQUIRE(ref == s);
    REQUIRE_FALSE(ref != s);
    REQUIRE(ref == s);
    REQUIRE_FALSE(ref != s);
    ref = data.blablabla;
    REQUIRE_FALSE(ref == s);
    REQUIRE(ref != s);
    ref = s.c_str();
    REQUIRE(ref <= s);
    REQUIRE(ref >= s);
    REQUIRE_FALSE(ref < s);
    REQUIRE_FALSE(ref > s);
    --ref[0];
    REQUIRE(ref < s);
    REQUIRE(ref <= s);
    REQUIRE_FALSE(ref > s);
    REQUIRE_FALSE(ref >= s);
    ref[0] = static_cast<CharType>(ref[0] + 2);
    REQUIRE(ref > s);
    REQUIRE(ref >= s);
    REQUIRE_FALSE(ref < s);
    REQUIRE_FALSE(ref <= s);
}


/* swap implementation */
TEMPLATE_LIST_TEST_CASE("StringBase swap", "[string_core]", StringBaseTestTypes)
{
    using StringType = TestType;
    using StorageType = typename StringType::storage_type;
    using CharType = typename StorageType::char_type;
    const TestData<CharType> data;

    StringType s1;

    // swap
    StringType s2(data.blablabla);
    REQUIRE(s1.empty());
    REQUIRE_FALSE(s2.empty());
    REQUIRE(s2 == data.blablabla);

    swap(s1, s2);
    REQUIRE(s2.empty());
    REQUIRE_FALSE(s1.empty());
    REQUIRE(s1 == data.blablabla);
    s2 = data.hello_world;
    swap(s2, s1);
    REQUIRE_FALSE(s1.empty());
    REQUIRE_FALSE(s2.empty());
    REQUIRE(s1 == data.hello_world);
    REQUIRE(s2 == data.blablabla);
}


/* hash specialization */
TEMPLATE_LIST_TEST_CASE("StringBase has", "[string_core]", StringBaseTestTypes)
{
    using StringType = TestType;
    using StorageType = typename StringType::storage_type;
    using CharType = typename StorageType::char_type;
    const TestData<CharType> data;

    StringType s1;
    std::hash<StringType> hash;

    // hash an empty string
    std::size_t val1 = hash(s1);
    std::size_t val2 = hash(s1);
    REQUIRE(val1 == val2);

    // hash a non-empty string - should have a different value
    s1 = data.hello_world;
    std::size_t val3 = hash(s1);
    std::size_t val4 = hash(s1);
    REQUIRE(val3 == val4);
    REQUIRE(val1 != val3);

    // hash another non-empty string - should have a different value
    s1 = data.blablabla;
    std::size_t val5 = hash(s1);
    std::size_t val6 = hash(s1);
    REQUIRE(val5 == val6);
    REQUIRE(val1 != val5);
    REQUIRE(val3 != val5);

    // repeat for every possible prefix
    while (s1.size() > 1)
    {
        s1.pop_back();
        std::size_t val7 = hash(s1);
        std::size_t val8 = hash(s1);
        REQUIRE(val7 == val8);
        REQUIRE(val1 != val7);
        REQUIRE(val3 != val7);
        REQUIRE(val5 != val7);
    }
}
