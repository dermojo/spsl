/**
 * @file	Special Purpose Strings Library: test_stringbase_erase.cpp
 * @author	Daniel Evers
 * @brief	StringBase unit tests: erase functions
 * @license MIT
 */

#include <sstream>

#include "catch.hpp"

#include "spsl.hpp"
#include "testdata.hpp"
#include "tests.hpp"


/* erase functions */
TEMPLATE_LIST_TEST_CASE("StringBase erase", "[string_base]", StringBaseTestTypes)
{
    using StringType = TestType;
    using StorageType = typename StringType::storage_type;
    using CharType = typename StorageType::char_type;
    const TestData<CharType> data;
    using RefType = std::basic_string<CharType>;

    // by index
    {
        StringType s(data.hello_world);
        RefType ref(data.hello_world);
        s.erase(2, 3);
        ref.erase(2, 3);
        REQUIRE(s == ref);

        // need to disambiguate since our iterators are actually plain pointers
        constexpr size_t pos = 0;
        s.erase(pos);
        ref.erase(0u);
        REQUIRE(s == ref);

        // no-op
        s.erase(s.length());
        REQUIRE(s == ref);

        REQUIRE_THROWS_AS(s.erase(s.length() + 1), std::out_of_range);
    }

    // single iterator
    {
        StringType s(data.hello_world);
        RefType ref(data.hello_world);
        auto sResult = s.erase(s.begin());
        auto refResult = ref.erase(ref.begin());
        REQUIRE(s == ref);
        REQUIRE(sResult == s.begin());
        REQUIRE(refResult == ref.begin());

        sResult = s.erase(s.cbegin() + 4);
        refResult = ref.erase(ref.begin() + 4);
        REQUIRE(s == ref);
        REQUIRE(sResult == s.begin() + 4);
        REQUIRE(refResult == ref.begin() + 4);

        REQUIRE_THROWS_AS(s.erase(s.end() + 1), std::out_of_range);
    }

    // iterator range
    {
        StringType s(data.hello_world);
        RefType ref(data.hello_world);
        auto sResult = s.erase(s.begin() + 1, s.begin() + 3);
        auto refResult = ref.erase(ref.begin() + 1, ref.begin() + 3);
        REQUIRE(s == ref);
        REQUIRE(sResult == s.begin() + 1);
        REQUIRE(refResult == ref.begin() + 1);

        sResult = s.erase(s.begin(), s.end());
        REQUIRE(s.empty());
        REQUIRE(sResult == s.begin());
        REQUIRE(sResult == s.end());

        s.assign(data.hello_world, data.hello_world_len);
        REQUIRE_THROWS_AS(s.erase(s.end() + 1, s.end() + 2), std::out_of_range);
        REQUIRE_THROWS_AS(s.erase(s.begin() - 1, s.end()), std::out_of_range);
        REQUIRE_THROWS_AS(s.erase(s.end(), s.begin()), std::out_of_range);
    }
}

/*
 * The following class templates are a big workaround for std::initializer_list problems on
 * Windows / MSVC. Here, we cannot use an initializer_list declared somewhere else, because
 * copying this instance results in unpredictable data - probably a bug somewhere in MSVC or their
 * STL. To avoid this, we explicitly write down the initializer list, but this requires that we
 * write the test twice - once for char, once for wchar_t.
 * And this is how the cookie crumbles...
 */

template <typename CharType, typename StringType>
class TestEraseInitializerList
{
};

template <typename StringType>
class TestEraseInitializerList<char, StringType>
{
public:
    void run()
    {
        TestData<char> data;
        using Traits = std::char_traits<char>;

        StringType s1(data.hello_world);
        std::string s2(data.hello_world);
        const size_t initListSize = 4;

        // same length
        s1.replace(s1.cbegin(), s1.cbegin() + initListSize, { 'T', 'e', 's', 't' });
        s2.replace(s2.begin(), s2.begin() + initListSize, { 'T', 'e', 's', 't' });
        REQUIRE(s1.size() == s2.size());
        REQUIRE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);

        // longer (forcing a reallocation for Password, but shorter than the max. array)
        s1.replace(s1.cbegin(), s1.cbegin() + 1, { 'T', 'e', 's', 't' });
        s2.replace(s2.begin(), s2.begin() + 1, { 'T', 'e', 's', 't' });
        REQUIRE(s1.size() == s2.size());
        REQUIRE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);

        // shorter
        s1.replace(s1.cbegin(), s1.cbegin() + initListSize + 3, { 'T', 'e', 's', 't' });
        s2.replace(s2.begin(), s2.begin() + initListSize + 3, { 'T', 'e', 's', 't' });
        REQUIRE(s1.size() == s2.size());
        REQUIRE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);

        // with offset
        s1.replace(s1.begin() + 3, s1.begin() + 5, { 'T', 'e', 's', 't' });
        s2.replace(s2.begin() + 3, s2.begin() + 5, { 'T', 'e', 's', 't' });
        REQUIRE(s1.size() == s2.size());
        REQUIRE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);
    }
};

template <typename StringType>
class TestEraseInitializerList<wchar_t, StringType>
{
public:
    void run()
    {
        TestData<wchar_t> data;
        using Traits = std::char_traits<wchar_t>;

        StringType s1(data.hello_world);
        std::wstring s2(data.hello_world);
        const size_t initListSize = 4;

        // same length
        s1.replace(s1.cbegin(), s1.cbegin() + initListSize, { L'T', L'e', L's', L't' });
        s2.replace(s2.begin(), s2.begin() + initListSize, { L'T', L'e', L's', L't' });
        REQUIRE(s1.size() == s2.size());
        REQUIRE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);

        // longer (forcing a reallocation for Password, but shorter than the max. array)
        s1.replace(s1.cbegin(), s1.cbegin() + 1, { L'T', L'e', L's', L't' });
        s2.replace(s2.begin(), s2.begin() + 1, { L'T', L'e', L's', L't' });
        REQUIRE(s1.size() == s2.size());
        REQUIRE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);

        // shorter
        s1.replace(s1.cbegin(), s1.cbegin() + initListSize + 3, { L'T', L'e', L's', L't' });
        s2.replace(s2.begin(), s2.begin() + initListSize + 3, { L'T', L'e', L's', L't' });
        REQUIRE(s1.size() == s2.size());
        REQUIRE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);

        // with offset
        s1.replace(s1.begin() + 3, s1.begin() + 5, { L'T', L'e', L's', L't' });
        s2.replace(s2.begin() + 3, s2.begin() + 5, { L'T', L'e', L's', L't' });
        REQUIRE(s1.size() == s2.size());
        REQUIRE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);
    }
};

template <typename StringType>
class TestEraseInitializerList<gsl::byte, StringType>
{
public:
    void run()
    {
        TestData<gsl::byte> data;
        TestData<char> sdata;
        using Traits = std::char_traits<gsl::byte>;

        StringType s1(data.hello_world);
        std::string s2(sdata.hello_world);
        const size_t initListSize = 4;

        // same length
        s1.replace(s1.cbegin(), s1.cbegin() + initListSize, { 'T'_b, 'e'_b, 's'_b, 't'_b });
        s2.replace(s2.begin(), s2.begin() + initListSize, { 'T', 'e', 's', 't' });
        REQUIRE(s1.size() == s2.size());
        REQUIRE(Traits::compare(s1.data(), reinterpret_cast<const gsl::byte*>(s2.data()),
                                s1.size()) == 0);

        // longer (forcing a reallocation for Password, but shorter than the max. array)
        s1.replace(s1.cbegin(), s1.cbegin() + 1, { 'T'_b, 'e'_b, 's'_b, 't'_b });
        s2.replace(s2.begin(), s2.begin() + 1, { 'T', 'e', 's', 't' });
        REQUIRE(s1.size() == s2.size());
        REQUIRE(Traits::compare(s1.data(), reinterpret_cast<const gsl::byte*>(s2.data()),
                                s1.size()) == 0);

        // shorter
        s1.replace(s1.cbegin(), s1.cbegin() + initListSize + 3, { 'T'_b, 'e'_b, 's'_b, 't'_b });
        s2.replace(s2.begin(), s2.begin() + initListSize + 3, { 'T', 'e', 's', 't' });
        REQUIRE(s1.size() == s2.size());
        REQUIRE(Traits::compare(s1.data(), reinterpret_cast<const gsl::byte*>(s2.data()),
                                s1.size()) == 0);

        // with offset
        s1.replace(s1.begin() + 3, s1.begin() + 5, { 'T'_b, 'e'_b, 's'_b, 't'_b });
        s2.replace(s2.begin() + 3, s2.begin() + 5, { 'T', 'e', 's', 't' });
        REQUIRE(s1.size() == s2.size());
        REQUIRE(Traits::compare(s1.data(), reinterpret_cast<const gsl::byte*>(s2.data()),
                                s1.size()) == 0);
    }
};

/* erase functions */
TEMPLATE_LIST_TEST_CASE("StringBase erase initializer_list", "[string_base]", StringBaseTestTypes)
{
    using StringType = TestType;
    using StorageType = typename StringType::storage_type;
    using CharType = typename StorageType::char_type;

    TestEraseInitializerList<CharType, StringType> test;
    test.run();
}
