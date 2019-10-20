/**
 * @file	Special Purpose Strings Library: test_stringcore_assign.cpp
 * @author	Daniel Evers
 * @brief	StringCore unit tests: assignment and append functions
 * @license MIT
 */

#include "catch.hpp"

#include "spsl.hpp"
#include "testdata.hpp"
#include "tests.hpp"


/* assignment operators */
TEMPLATE_LIST_TEST_CASE("StringCore assignment operators", "[string_core]", StringCoreTestTypes)
{
    using StringType = TestType;
    using StorageType = typename StringType::storage_type;
    using CharType = typename StorageType::char_type;
    using Traits = typename StringType::traits_type;
    const TestData<CharType> data;
    std::initializer_list<CharType> initList = data.initializerList();

    StringType s;

    // const char_type*
    s = data.hello_world;
    REQUIRE(s.size() == data.hello_world_len);
    REQUIRE(Traits::compare(s.c_str(), data.hello_world, data.hello_world_len) == 0);
    REQUIRE(s == data.hello_world);
    s = data.blablabla;
    REQUIRE(s.size() == data.blablabla_len);
    REQUIRE(Traits::compare(s.c_str(), data.blablabla, data.blablabla_len) == 0);
    REQUIRE(s == data.blablabla);

    // char_type
    const CharType ch = data.hello_world[0];
    s = ch;
    REQUIRE(s.size() == 1u);
    REQUIRE(Traits::compare(s.c_str(), &ch, 1) == 0);
    REQUIRE(s[0] == ch);

    // initializer list
    s = initList;
    REQUIRE(s.size() == initList.size());
    REQUIRE(Traits::compare(s.c_str(), initList.begin(), initList.size()) == 0);

    // copy
    StringType s2(data.blablabla);
    s = s2;
    REQUIRE(s.size() == data.blablabla_len);
    REQUIRE(Traits::compare(s.c_str(), data.blablabla, data.blablabla_len) == 0);
    REQUIRE(s == data.blablabla);

    // move
    s2 = data.hello_world;
    s = std::move(s2);
    REQUIRE(s.size() == data.hello_world_len);
    REQUIRE(Traits::compare(s.c_str(), data.hello_world, data.hello_world_len) == 0);
    REQUIRE(s == data.hello_world);

    // other string classes
    // 1. std::basic_string
    std::basic_string<CharType> str(data.blablabla);
    s = str;
    REQUIRE(s.size() == data.blablabla_len);
    REQUIRE(Traits::compare(s.c_str(), data.blablabla, data.blablabla_len) == 0);
    REQUIRE(s == data.blablabla);
    // 2. std::vector
    std::vector<CharType> vec;
    vec.resize(data.hello_world_len);
    Traits::copy(vec.data(), data.hello_world, data.hello_world_len);
    s = vec;
    REQUIRE(s.size() == data.hello_world_len);
    REQUIRE(Traits::compare(s.c_str(), data.hello_world, data.hello_world_len) == 0);
    REQUIRE(s == data.hello_world);
// 3. basic_string_view if available
#ifdef TEST_STRING_VIEW
    std::experimental::basic_string_view<CharType> view(data.blablabla, data.blablabla_len);
    s = view;
    REQUIRE(s.size() == data.blablabla_len);
    REQUIRE(Traits::compare(s.c_str(), data.blablabla, data.blablabla_len) == 0);
    REQUIRE(s == data.blablabla);
#endif
}


/* assignment functions */
TEMPLATE_LIST_TEST_CASE("StringCore assignment", "[string_core]", StringCoreTestTypes)
{
    using StringType = TestType;
    using StorageType = typename StringType::storage_type;
    using CharType = typename StorageType::char_type;
    using Traits = typename StringType::traits_type;
    const TestData<CharType> data;
    std::initializer_list<CharType> initList = data.initializerList();
    const CharType nul = StorageType::nul();

    StringType s;
    const CharType ch = data.hello_world[0];

    // count + character
    s.assign(42, ch);
    REQUIRE(s.size() == 42u);
    for (auto c : s)
        REQUIRE(c == ch);
    for (size_t i = 0; i < s.size(); ++i)
        REQUIRE(s[i] == ch);
    REQUIRE(s[s.size()] == nul);

    // const char_type* + length
    s.assign(data.blablabla, data.blablabla_len);
    REQUIRE(s.size() == data.blablabla_len);
    REQUIRE(Traits::compare(s.c_str(), data.blablabla, data.blablabla_len) == 0);
    REQUIRE(s == data.blablabla);

    // const char_type*
    s.assign(data.hello_world);
    REQUIRE(s.size() == data.hello_world_len);
    REQUIRE(Traits::compare(s.c_str(), data.hello_world, data.hello_world_len) == 0);
    REQUIRE(s == data.hello_world);
    s.assign(data.blablabla);
    REQUIRE(s.size() == data.blablabla_len);
    REQUIRE(Traits::compare(s.c_str(), data.blablabla, data.blablabla_len) == 0);
    REQUIRE(s == data.blablabla);

    // initializer list
    s.assign(initList);
    REQUIRE(s.size() == initList.size());
    REQUIRE(Traits::compare(s.c_str(), initList.begin(), initList.size()) == 0);

    // iterators
    const std::basic_string<CharType> str(data.blablabla);
    s.assign(str.begin(), str.end());
    REQUIRE(s.size() == data.blablabla_len);
    REQUIRE(Traits::compare(s.c_str(), data.blablabla, data.blablabla_len) == 0);
    REQUIRE(s == data.blablabla);

    // copy
    StringType s2(data.blablabla);
    s.assign(s2);
    REQUIRE(s.size() == data.blablabla_len);
    REQUIRE(Traits::compare(s.c_str(), data.blablabla, data.blablabla_len) == 0);
    REQUIRE(s == data.blablabla);

    // move
    s2 = data.hello_world;
    s.assign(std::move(s2));
    REQUIRE(s.size() == data.hello_world_len);
    REQUIRE(Traits::compare(s.c_str(), data.hello_world, data.hello_world_len) == 0);
    REQUIRE(s == data.hello_world);

    // other string classes
    // 1. std::basic_string (reuse the object from above)
    s.assign(str);
    REQUIRE(s.size() == data.blablabla_len);
    REQUIRE(Traits::compare(s.c_str(), data.blablabla, data.blablabla_len) == 0);
    REQUIRE(s == data.blablabla);
    // 2. std::vector
    std::vector<CharType> vec;
    vec.resize(data.hello_world_len);
    Traits::copy(vec.data(), data.hello_world, data.hello_world_len);
    s.assign(vec);
    REQUIRE(s.size() == data.hello_world_len);
    REQUIRE(Traits::compare(s.c_str(), data.hello_world, data.hello_world_len) == 0);
    REQUIRE(s == data.hello_world);
// 3. basic_string_view if available
#ifdef TEST_STRING_VIEW
    std::experimental::basic_string_view<CharType> view(data.blablabla, data.blablabla_len);
    s.assign(view);
    REQUIRE(s.size() == data.blablabla_len);
    REQUIRE(Traits::compare(s.c_str(), data.blablabla, data.blablabla_len) == 0);
    REQUIRE(s == data.blablabla);
#endif

    // other string class with pos and count
    s.assign(str, 1, 3);
    const auto sub = str.substr(1, 3);
    REQUIRE(s.size() == sub.size());
    REQUIRE(Traits::compare(s.c_str(), sub.data(), sub.size()) == 0);
    REQUIRE(s == sub);

    // special case: empty range
    s.assign(str, str.size(), StringType::npos);
    REQUIRE(s.size() == 0u);
    REQUIRE(s.empty());

    // throws if pos is out of range
    REQUIRE_THROWS_AS(s.assign(str, str.size() + 1), std::out_of_range);
}

/* append functions */
TEMPLATE_LIST_TEST_CASE("StringCore append", "[string_core]", StringCoreTestTypes)
{
    using StringType = TestType;
    using StorageType = typename StringType::storage_type;
    using CharType = typename StorageType::char_type;
    using Traits = typename StringType::traits_type;
    const TestData<CharType> data;
    std::initializer_list<CharType> initList = data.initializerList();
    const CharType ch = data.hello_world[5];

    // we perform all operations simultaneously on both string types for easier comparison
    StringType s1;
    std::basic_string<CharType> s2;

    // count, char
    s1.append(17, ch);
    s2.append(17, ch);
    REQUIRE(s1 == s2);
    REQUIRE(s1.size() == s2.size());
    REQUIRE(Traits::compare(s1.c_str(), s2.c_str(), s1.size()) == 0);

    // char_type*, count
    s1.append(data.blablabla, data.blablabla_len);
    s2.append(data.blablabla, data.blablabla_len);
    REQUIRE(s1 == s2);
    REQUIRE(s1.size() == s2.size());
    REQUIRE(Traits::compare(s1.c_str(), s2.c_str(), s1.size()) == 0);

    // char_type*
    s1.append(data.hello_world);
    s2.append(data.hello_world);
    REQUIRE(s1 == s2);
    REQUIRE(s1.size() == s2.size());
    REQUIRE(Traits::compare(s1.c_str(), s2.c_str(), s1.size()) == 0);

    s1.clear();
    s2.clear();

    // initializer list
    s1.append(initList);
    s2.append(initList);
    REQUIRE(s1 == s2);
    REQUIRE(s1.size() == s2.size());
    REQUIRE(Traits::compare(s1.c_str(), s2.c_str(), s1.size()) == 0);

    // iterator range
    const std::vector<CharType> vec(23, ch);
    s1.append(vec);
    // std::basic_string can' do this
    s2.append(23, ch);
    REQUIRE(s1 == s2);
    REQUIRE(s1.size() == s2.size());
    REQUIRE(Traits::compare(s1.c_str(), s2.c_str(), s1.size()) == 0);

    // other string class
    const std::basic_string<CharType> other(data.blablabla);
    s1.append(other);
    s2.append(other);
    REQUIRE(s1 == s2);
    REQUIRE(s1.size() == s2.size());
    REQUIRE(Traits::compare(s1.c_str(), s2.c_str(), s1.size()) == 0);

    s1.clear();
    s2.clear();

    // other string class with pos and count
    s1.append(other, 2, other.size() - 3);
    s2.append(other, 2, other.size() - 3);
    REQUIRE(s1 == s2);
    REQUIRE(s1.size() == s2.size());
    REQUIRE(Traits::compare(s1.c_str(), s2.c_str(), s1.size()) == 0);

    // += char_type
    s1 += ch;
    s2 += ch;
    REQUIRE(s1 == s2);
    REQUIRE(s1.size() == s2.size());
    REQUIRE(Traits::compare(s1.c_str(), s2.c_str(), s1.size()) == 0);

    // += char_type*
    s1 += data.hello_world;
    s2 += data.hello_world;
    REQUIRE(s1 == s2);
    REQUIRE(s1.size() == s2.size());
    REQUIRE(Traits::compare(s1.c_str(), s2.c_str(), s1.size()) == 0);

    // += initializer_list
    s1 += initList;
    s2 += initList;
    REQUIRE(s1 == s2);
    REQUIRE(s1.size() == s2.size());
    REQUIRE(Traits::compare(s1.c_str(), s2.c_str(), s1.size()) == 0);

    s1.clear();
    s2.clear();

    // += other string class
    s1 += other;
    s2 += other;
    REQUIRE(s1 == s2);
    REQUIRE(s1.size() == s2.size());
    REQUIRE(Traits::compare(s1.c_str(), s2.c_str(), s1.size()) == 0);

    // special case: empty range
    s1.append(s2, s2.size(), StringType::npos);
    // still the same content
    REQUIRE(s1 == s2);
    REQUIRE(s1.size() == s2.size());
    REQUIRE(Traits::compare(s1.c_str(), s2.c_str(), s1.size()) == 0);

    // throws if pos is out of range
    REQUIRE_THROWS_AS(s1.append(s2, s2.size() + 1), std::out_of_range);
}
