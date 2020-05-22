/**
 * @file	Special Purpose Strings Library: test_stringbase_replace.cpp
 * @author	Daniel Evers
 * @brief	StringBase unit tests: replace functions
 * @license MIT
 */

#include <sstream>

#include "doctest.h"

#include "spsl.hpp"
#include "testdata.hpp"
#include "tests.hpp"


/*
 * The following class templates are a big workaround for std::initializer_list problems on
 * Windows / MSVC. Here, we cannot use an initializer_list declared somewhere else, because
 * copying this instance results in unpredictable data - probably a bug somewhere in MSVC or their
 * STL. To avoid this, we explicitly write down the initializer list, but this requires that we
 * write the test twice - once for char, once for wchar_t.
 * And this is how the cookie crumbles...
 */

template <typename CharType, typename StringType>
class TestReplaceInitializerList
{
};

template <typename StringType>
class TestReplaceInitializerList<char, StringType>
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
class TestReplaceInitializerList<wchar_t, StringType>
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
class TestReplaceInitializerList<gsl::byte, StringType>
{
public:
    void run()
    {
        TestData<gsl::byte> data;
        TestData<char> sdata;
        using Traits = std::char_traits<gsl::byte>;

        StringType s1(data.hello_world, data.hello_world_len);
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


/* replace functions */
TEST_CASE_TEMPLATE_DEFINE("StringBase replace", StringType, StringBase_replace)
{
    using StorageType = typename StringType::storage_type;
    using CharType = typename StorageType::char_type;
    using Traits = typename StorageType::traits_type;
    const TestData<CharType> data;
    const auto npos = StringType::npos;

    using RefType = std::basic_string<CharType>;

    // replace(size_type pos, size_type count, const this_type& str);
    {
        StringType s1(data.hello_world);
        RefType s2(data.hello_world);
        StringType repl1;
        RefType repl2;

        // same length
        repl1.assign(data.blablabla, 3);
        repl2.assign(data.blablabla, 3);
        s1.replace(0, 3, repl1);
        s2.replace(0, 3, repl2);
        REQUIRE(s1.size() == s2.size());
        REQUIRE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);

        // longer (forcing a reallocation for Password, but shorter than the max. array)
        repl1.assign(32, data.blablabla[0]);
        repl2.assign(32, data.blablabla[0]);
        s1.replace(0, 3, repl1);
        s2.replace(0, 3, repl2);
        REQUIRE(s1.size() == s2.size());
        REQUIRE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);

        // shorter
        repl1.assign(1, data.blablabla[0]);
        repl2.assign(1, data.blablabla[0]);
        s1.replace(0, 3, repl1);
        s2.replace(0, 3, repl2);
        REQUIRE(s1.size() == s2.size());
        REQUIRE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);

        // replace all
        repl1.clear();
        repl2.clear();
        s1.replace(0, s1.size(), repl1);
        s2.replace(0, s2.size(), repl2);
        REQUIRE(s1.size() == s2.size());
        REQUIRE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);
        REQUIRE(s1.empty());
        REQUIRE(s2.empty());

        // exceptions
        // (1) invalid pos raises out_of_range
        REQUIRE_THROWS_AS(s1.replace(s1.size(), s1.size() + 3, repl1), std::out_of_range);
        REQUIRE_THROWS_AS(s1.replace(s1.size() + 1, s1.size() + 3, repl1), std::out_of_range);
        // (2) in our class, an invalid count does the same
        REQUIRE_THROWS_AS(s1.replace(0, s1.size() + 1, repl1), std::out_of_range);
    }

    // replace(const_iterator first, const_iterator last, const this_type& str);
    {
        StringType s1(data.hello_world);
        RefType s2(data.hello_world);
        StringType repl1;
        RefType repl2;

        // same length
        repl1.assign(data.blablabla, 3);
        repl2.assign(data.blablabla, 3);
        s1.replace(s1.cbegin(), s1.cbegin() + 3, repl1);
        s2.replace(s2.begin(), s2.begin() + 3, repl2);
        REQUIRE(s1.size() == s2.size());
        REQUIRE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);

        // longer (forcing a reallocation for Password, but shorter than the max. array)
        repl1.assign(32, data.blablabla[0]);
        repl2.assign(32, data.blablabla[0]);
        s1.replace(s1.cbegin(), s1.cbegin() + 3, repl1);
        s2.replace(s2.begin(), s2.begin() + 3, repl2);
        REQUIRE(s1.size() == s2.size());
        REQUIRE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);

        // shorter
        repl1.assign(1, data.blablabla[0]);
        repl2.assign(1, data.blablabla[0]);
        s1.replace(s1.cbegin(), s1.cbegin() + 3, repl1);
        s2.replace(s2.begin(), s2.begin() + 3, repl2);
        REQUIRE(s1.size() == s2.size());
        REQUIRE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);

        // replace all
        repl1.clear();
        repl2.clear();
        s1.replace(s1.cbegin(), s1.cend(), repl1);
        s2.replace(s2.begin(), s2.end(), repl2);
        REQUIRE(s1.size() == s2.size());
        REQUIRE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);
        REQUIRE(s1.empty());
        REQUIRE(s2.empty());

        // exceptions
        // (1) invalid 'first' raises out_of_range
        REQUIRE_THROWS_AS(s1.replace(s1.end() + 1, s1.end() + 2, repl1), std::out_of_range);
        // (2) in our class, an invalid 'last' does the same
        REQUIRE_THROWS_AS(s1.replace(s1.begin(), s1.end() + 1, repl1), std::out_of_range);
    }

    // replace(size_type pos, size_type count, const this_type& str, size_type pos2, size_type
    // count2 = npos);
    {
        StringType repl1;
        RefType repl2;

        // same length
        repl1.assign(data.blablabla, 3);
        repl2.assign(data.blablabla, 3);
        std::array<size_t, 2> sizes1{ 3, npos };
        for (auto len : sizes1)
        {
            StringType s1(data.hello_world);
            RefType s2(data.hello_world);
            s1.replace(0, 3, repl1, 0, len);
            s2.replace(0, 3, repl2, 0, len);
            REQUIRE(s1.size() == s2.size());
            REQUIRE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);
        }

        // longer (forcing a reallocation for Password, but shorter than the max. array)
        repl1.assign(32, data.blablabla[0]);
        repl2.assign(32, data.blablabla[0]);
        std::array<size_t, 2> sizes2{ 32, npos };
        for (auto len : sizes2)
        {
            StringType s1(data.hello_world);
            RefType s2(data.hello_world);
            s1.replace(0, 3, repl1, 0, len);
            s2.replace(0, 3, repl2, 0, len);
            REQUIRE(s1.size() == s2.size());
            REQUIRE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);
        }

        // shorter
        repl1.assign(1, data.blablabla[0]);
        repl2.assign(1, data.blablabla[0]);
        std::array<size_t, 2> sizes3{ 1, npos };
        for (auto len : sizes3)
        {
            StringType s1(data.hello_world);
            RefType s2(data.hello_world);
            s1.replace(0, 3, repl1, 0, len);
            s2.replace(0, 3, repl2, 0, len);
            REQUIRE(s1.size() == s2.size());
            REQUIRE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);
        }

        // various lengths, but with pos2 != 0
        repl1 = data.blablabla;
        repl2 = data.blablabla;
        std::array<size_t, 2> sizes4{ 3, npos };
        for (auto len : sizes4)
        {
            StringType s1(data.hello_world);
            RefType s2(data.hello_world);
            s1.replace(0, 3, repl1, 3, len);
            s2.replace(0, 3, repl2, 3, len);
            REQUIRE(s1.size() == s2.size());
            REQUIRE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);
        }

        // replace all
        repl1.clear();
        repl2.clear();
        std::array<size_t, 2> sizes5{ 0, npos };
        for (auto len : sizes5)
        {
            StringType s1(data.hello_world);
            RefType s2(data.hello_world);
            s1.replace(0, s1.size(), repl1, 0, len);
            s2.replace(0, s2.size(), repl2, 0, len);
            REQUIRE(s1.size() == s2.size());
            REQUIRE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);
            REQUIRE(s1.empty());
            REQUIRE(s2.empty());
        }

        // exceptions: verify that they are thrown and that the string doesn't change
        {
            StringType s1(data.hello_world);
            // (1) invalid pos raises out_of_range
            REQUIRE_THROWS_AS(s1.replace(s1.size(), s1.size() + 3, repl1, 0, repl1.size()),
                              std::out_of_range);
            REQUIRE(s1 == data.hello_world);
            // (2) in our class, an invalid count does the same
            REQUIRE_THROWS_AS(s1.replace(0, s1.size() + 1, repl1, 0, repl1.size()),
                              std::out_of_range);
            REQUIRE(s1 == data.hello_world);
        }
    }

    // replace(const_iterator first, const_iterator last, InputIt first2, InputIt last2);
    {
        StringType s1(data.hello_world);
        RefType s2(data.hello_world);
        StringType repl1;
        RefType repl2;

        // same length
        repl1.assign(data.blablabla, 3);
        repl2.assign(data.blablabla, 3);
        s1.replace(s1.cbegin(), s1.cbegin() + 3, repl1.begin(), repl1.end());
        s2.replace(s2.begin(), s2.begin() + 3, repl2.begin(), repl2.end());
        REQUIRE(s1.size() == s2.size());
        REQUIRE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);
        // reverse_iterator
        s1.replace(s1.cbegin(), s1.cbegin() + 3, repl1.crbegin(), repl1.crend());
        s2.replace(s2.begin(), s2.begin() + 3, repl2.crbegin(), repl2.crend());
        REQUIRE(s1.size() == s2.size());
        REQUIRE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);

        // longer (forcing a reallocation for Password, but shorter than the max. array)
        repl1.assign(32, data.blablabla[0]);
        repl2.assign(32, data.blablabla[0]);
        s1.replace(s1.cbegin(), s1.cbegin() + 3, repl1.begin(), repl1.end());
        s2.replace(s2.begin(), s2.begin() + 3, repl2.begin(), repl2.end());
        REQUIRE(s1.size() == s2.size());
        REQUIRE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);
        // again, using std::vector for our string class
        std::vector<CharType> vec(32, data.blablabla[0]);
        s1 = data.hello_world;
        s2 = data.hello_world;
        s1.replace(s1.cbegin(), s1.cbegin() + 3, vec.begin(), vec.end());
        s2.replace(s2.begin(), s2.begin() + 3, repl2.begin(), repl2.end());
        REQUIRE(s1.size() == s2.size());
        REQUIRE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);

        // shorter
        repl1.assign(1, data.blablabla[0]);
        repl2.assign(1, data.blablabla[0]);
        s1.replace(s1.cbegin(), s1.cbegin() + 3, repl1.begin(), repl1.end());
        s2.replace(s2.begin(), s2.begin() + 3, repl2.begin(), repl2.end());
        REQUIRE(s1.size() == s2.size());
        REQUIRE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);

        // replace all
        repl1.clear();
        repl2.clear();
        s1.replace(s1.begin(), s1.end(), repl1.begin(), repl1.end());
        s2.replace(s2.begin(), s2.end(), repl2.begin(), repl2.end());
        REQUIRE(s1.size() == s2.size());
        REQUIRE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);
        REQUIRE(s1.empty());
        REQUIRE(s2.empty());

        // exceptions: verify that they are thrown and that the string doesn't change
        {
            s1 = data.hello_world;
            // (1) invalid pos raises out_of_range
            REQUIRE_THROWS_AS(s1.replace(s1.end() + 1, s1.end(), repl1.begin(), repl1.end()),
                              std::out_of_range);
            REQUIRE(s1 == data.hello_world);
            // (2) in our class, an invalid count does the same
            REQUIRE_THROWS_AS(s1.replace(s1.begin(), s1.end() + 1, repl1.begin(), repl1.end()),
                              std::out_of_range);
            REQUIRE(s1 == data.hello_world);
        }
    }

    // replace(size_type pos, size_type count, const char_type* cstr, size_type count2);
    {
        StringType repl1;
        RefType repl2;

        // same length
        repl1.assign(data.blablabla, 3);
        repl2.assign(data.blablabla, 3);
        std::array<size_t, 1> sizes1{ 3 };
        for (auto len : sizes1)
        {
            StringType s1(data.hello_world);
            RefType s2(data.hello_world);
            s1.replace(0, 3, repl1.data(), len);
            s2.replace(0, 3, repl2.data(), len);
            REQUIRE(s1.size() == s2.size());
            REQUIRE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);
        }

        // longer (forcing a reallocation for Password, but shorter than the max. array)
        repl1.assign(32, data.blablabla[0]);
        repl2.assign(32, data.blablabla[0]);
        std::array<size_t, 2> sizes2{ 13, 32 };
        for (auto len : sizes2)
        {
            StringType s1(data.hello_world);
            RefType s2(data.hello_world);
            s1.replace(0, 3, repl1.data(), len);
            s2.replace(0, 3, repl2.data(), len);
            REQUIRE(s1.size() == s2.size());
            REQUIRE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);
        }

        // shorter
        repl1.assign(1, data.blablabla[0]);
        repl2.assign(1, data.blablabla[0]);
        std::array<size_t, 1> sizes3{ 1 };
        for (auto len : sizes3)
        {
            StringType s1(data.hello_world);
            RefType s2(data.hello_world);
            s1.replace(0, 3, repl1.data(), len);
            s2.replace(0, 3, repl2.data(), len);
            REQUIRE(s1.size() == s2.size());
            REQUIRE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);
        }

        // various lengths, but with offset
        repl1 = data.blablabla;
        repl2 = data.blablabla;
        std::array<size_t, 2> sizes4{ 3, data.blablabla_len };
        for (auto len : sizes4)
        {
            StringType s1(data.hello_world);
            RefType s2(data.hello_world);
            s1.replace(1, 3, repl1.data() + 3, len);
            s2.replace(1, 3, repl2.data() + 3, len);
            REQUIRE(s1.size() == s2.size());
            REQUIRE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);
        }

        // replace all
        repl1.clear();
        repl2.clear();
        std::array<size_t, 1> sizes5{ 0 };
        for (auto len : sizes5)
        {
            StringType s1(data.hello_world);
            RefType s2(data.hello_world);
            s1.replace(0, s1.size(), repl1.data(), len);
            s2.replace(0, s2.size(), repl2.data(), len);
            REQUIRE(s1.size() == s2.size());
            REQUIRE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);
            REQUIRE(s1.empty());
            REQUIRE(s2.empty());
        }

        // exceptions: verify that they are thrown and that the string doesn't change
        {
            StringType s1(data.hello_world);
            // (1) invalid pos raises out_of_range
            REQUIRE_THROWS_AS(s1.replace(s1.size(), s1.size() + 3, repl1.data(), repl1.size()),
                              std::out_of_range);
            REQUIRE(s1 == data.hello_world);
            // (2) in our class, an invalid count does the same
            REQUIRE_THROWS_AS(s1.replace(0, s1.size() + 1, repl1.data(), repl1.size()),
                              std::out_of_range);
            REQUIRE(s1 == data.hello_world);
        }
    }

    // replace(const_iterator first, const_iterator last, const char_type* cstr, size_type count2);
    {
        StringType repl1;
        RefType repl2;

        // same length
        repl1.assign(data.blablabla, 3);
        repl2.assign(data.blablabla, 3);
        std::array<size_t, 1> sizes1{ 3 };
        for (auto len : sizes1)
        {
            StringType s1(data.hello_world);
            RefType s2(data.hello_world);
            s1.replace(s1.cbegin(), s1.cbegin() + 3, repl1.data(), len);
            s2.replace(s2.begin(), s2.begin() + 3, repl2.data(), len);
            REQUIRE(s1.size() == s2.size());
            REQUIRE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);
        }

        // longer (forcing a reallocation for Password, but shorter than the max. array)
        repl1.assign(32, data.blablabla[0]);
        repl2.assign(32, data.blablabla[0]);
        std::array<size_t, 2> sizes2{ 13, 32 };
        for (auto len : sizes2)
        {
            StringType s1(data.hello_world);
            RefType s2(data.hello_world);
            s1.replace(s1.cbegin(), s1.cbegin() + 3, repl1.data(), len);
            s2.replace(s2.begin(), s2.begin() + 3, repl2.data(), len);
            REQUIRE(s1.size() == s2.size());
            REQUIRE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);
        }

        // shorter
        repl1.assign(1, data.blablabla[0]);
        repl2.assign(1, data.blablabla[0]);
        std::array<size_t, 1> sizes3{ 1 };
        for (auto len : sizes3)
        {
            StringType s1(data.hello_world);
            RefType s2(data.hello_world);
            s1.replace(s1.cbegin(), s1.cbegin() + 3, repl1.data(), len);
            s2.replace(s2.begin(), s2.begin() + 3, repl2.data(), len);
            REQUIRE(s1.size() == s2.size());
            REQUIRE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);
        }

        // various lengths, but with offsets
        repl1 = data.blablabla;
        repl2 = data.blablabla;
        std::array<size_t, 2> sizes4{ 3, data.blablabla_len };
        for (auto len : sizes4)
        {
            StringType s1(data.hello_world);
            RefType s2(data.hello_world);
            s1.replace(s1.cbegin() + 1, s1.cbegin() + 5, repl1.data() + 3, len);
            s2.replace(s2.begin() + 1, s2.begin() + 5, repl2.data() + 3, len);
            REQUIRE(s1.size() == s2.size());
            REQUIRE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);
        }

        // replace all
        repl1.clear();
        repl2.clear();
        std::array<size_t, 1> sizes5{ 0 };
        for (auto len : sizes5)
        {
            StringType s1(data.hello_world);
            RefType s2(data.hello_world);
            s1.replace(s1.begin(), s1.end(), repl1.data(), len);
            s2.replace(s2.begin(), s2.end(), repl2.data(), len);
            REQUIRE(s1.size() == s2.size());
            REQUIRE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);
            REQUIRE(s1.empty());
            REQUIRE(s2.empty());
        }
    }

    // replace(size_type pos, size_type count, const char_type* cstr);
    {
        StringType s1(data.hello_world);
        RefType s2(data.hello_world);
        StringType repl1;
        RefType repl2;

        // same length
        repl1.assign(data.blablabla, 3);
        repl2.assign(data.blablabla, 3);
        s1.replace(0, 3, repl1.data());
        s2.replace(0, 3, repl2.data());
        REQUIRE(s1.size() == s2.size());
        REQUIRE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);

        // longer (forcing a reallocation for Password, but shorter than the max. array)
        repl1.assign(32, data.blablabla[0]);
        repl2.assign(32, data.blablabla[0]);
        s1.replace(0, 3, repl1.data());
        s2.replace(0, 3, repl2.data());
        REQUIRE(s1.size() == s2.size());
        REQUIRE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);

        // shorter
        repl1.assign(1, data.blablabla[0]);
        repl2.assign(1, data.blablabla[0]);
        s1.replace(0, 3, repl1.data());
        s2.replace(0, 3, repl2.data());
        REQUIRE(s1.size() == s2.size());
        REQUIRE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);

        // with offset
        repl1 = data.blablabla;
        repl2 = data.blablabla;
        s1 = data.hello_world;
        s2 = data.hello_world;
        s1.replace(3, 7, repl1.data());
        s2.replace(3, 7, repl2.data());
        REQUIRE(s1.size() == s2.size());
        REQUIRE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);

        // replace all
        repl1.clear();
        repl2.clear();
        s1.replace(0, s1.size(), repl1.data());
        s2.replace(0, s2.size(), repl2.data());
        REQUIRE(s1.size() == s2.size());
        REQUIRE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);
        REQUIRE(s1.empty());
        REQUIRE(s2.empty());

        // exceptions: verify that they are thrown and that the string doesn't change
        {
            StringType s3(data.hello_world);
            // (1) invalid pos raises out_of_range
            REQUIRE_THROWS_AS(s3.replace(s3.size(), s3.size() + 3, repl1.data()),
                              std::out_of_range);
            REQUIRE(s3 == data.hello_world);
            // (2) in our class, an invalid count does the same
            REQUIRE_THROWS_AS(s3.replace(0, s3.size() + 1, repl1.data()), std::out_of_range);
            REQUIRE(s3 == data.hello_world);
        }
    }

    // replace(const_iterator first, const_iterator last, const char_type* cstr);
    {
        StringType s1(data.hello_world);
        RefType s2(data.hello_world);
        StringType repl1;
        RefType repl2;

        // same length
        repl1.assign(data.blablabla, 3);
        repl2.assign(data.blablabla, 3);
        s1.replace(s1.cbegin(), s1.cbegin() + 3, repl1.data());
        s2.replace(s2.begin(), s2.begin() + 3, repl2.data());
        REQUIRE(s1.size() == s2.size());
        REQUIRE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);

        // longer (forcing a reallocation for Password, but shorter than the max. array)
        repl1.assign(32, data.blablabla[0]);
        repl2.assign(32, data.blablabla[0]);
        s1.replace(s1.cbegin(), s1.cbegin() + 3, repl1.data());
        s2.replace(s2.begin(), s2.begin() + 3, repl2.data());
        REQUIRE(s1.size() == s2.size());
        REQUIRE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);

        // shorter
        repl1.assign(1, data.blablabla[0]);
        repl2.assign(1, data.blablabla[0]);
        s1.replace(s1.cbegin(), s1.cbegin() + 3, repl1.data());
        s2.replace(s2.begin(), s2.begin() + 3, repl2.data());
        REQUIRE(s1.size() == s2.size());
        REQUIRE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);

        // with offset
        repl1 = data.blablabla;
        repl2 = data.blablabla;
        s1 = data.hello_world;
        s2 = data.hello_world;
        s1.replace(s1.begin() + 3, s1.begin() + 7, repl1.data());
        s2.replace(s2.begin() + 3, s2.begin() + 7, repl2.data());
        REQUIRE(s1.size() == s2.size());
        REQUIRE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);

        // replace all
        repl1.clear();
        repl2.clear();
        s1.replace(s1.begin(), s1.end(), repl1.data());
        s2.replace(s2.begin(), s2.end(), repl2.data());
        REQUIRE(s1.size() == s2.size());
        REQUIRE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);
        REQUIRE(s1.empty());
        REQUIRE(s2.empty());
    }

    // replace(size_type pos, size_type count, size_type count2, char_type ch);
    {
        StringType s1(data.hello_world);
        RefType s2(data.hello_world);
        const CharType repl = data.blablabla[1];

        // same length
        s1.replace(0, 3, 3, repl);
        s2.replace(0, 3, 3, repl);
        REQUIRE(s1.size() == s2.size());
        REQUIRE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);

        // longer (forcing a reallocation for Password, but shorter than the max. array)
        s1.replace(0, 3, 32, repl);
        s2.replace(0, 3, 32, repl);
        REQUIRE(s1.size() == s2.size());
        REQUIRE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);

        // shorter
        s1.replace(0, 12, 1, repl);
        s2.replace(0, 12, 1, repl);
        REQUIRE(s1.size() == s2.size());
        REQUIRE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);

        // various lengths, but with offset
        std::array<size_t, 5> sizes4{ 0, 1, 3, 5, data.blablabla_len };
        for (auto len : sizes4)
        {
            StringType s3(data.hello_world);
            RefType s4(data.hello_world);
            s3.replace(3, 4, len, repl);
            s4.replace(3, 4, len, repl);
            REQUIRE(s3.size() == s4.size());
            REQUIRE(Traits::compare(s3.data(), s4.data(), s3.size()) == 0);
        }

        // replace all
        s1.replace(0, s1.size(), 0, repl);
        s2.replace(0, s2.size(), 0, repl);
        REQUIRE(s1.size() == s2.size());
        REQUIRE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);
        REQUIRE(s1.empty());
        REQUIRE(s2.empty());

        // exceptions: verify that they are thrown and that the string doesn't change
        {
            s1 = data.hello_world;
            // (1) invalid pos raises out_of_range
            REQUIRE_THROWS_AS(s1.replace(s1.size() + 1, 3, 3, repl), std::out_of_range);
            REQUIRE(s1 == data.hello_world);
            // (2) in our class, an invalid count does the same
            REQUIRE_THROWS_AS(s1.replace(0, s1.size() + 1, 3, repl), std::out_of_range);
            REQUIRE(s1 == data.hello_world);
        }
    }

    // replace(const_iterator first, const_iterator last, size_type count2, char_type ch);
    {
        StringType s1(data.hello_world);
        RefType s2(data.hello_world);
        const CharType repl = data.blablabla[1];

        // same length
        s1.replace(s1.cbegin(), s1.cbegin() + 3, 3, repl);
        s2.replace(s2.begin(), s2.begin() + 3, 3, repl);
        REQUIRE(s1.size() == s2.size());
        REQUIRE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);

        // longer (forcing a reallocation for Password, but shorter than the max. array)
        s1.replace(s1.cbegin(), s1.cbegin() + 3, 32, repl);
        s2.replace(s2.begin(), s2.begin() + 3, 32, repl);
        REQUIRE(s1.size() == s2.size());
        REQUIRE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);

        // shorter
        s1.replace(s1.cbegin(), s1.cbegin() + 12, 1, repl);
        s2.replace(s2.begin(), s2.begin() + 12, 1, repl);
        REQUIRE(s1.size() == s2.size());
        REQUIRE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);

        // various lengths, but with offset
        std::array<size_t, 5> sizes4{ 0, 1, 3, 5, data.blablabla_len };
        for (auto len : sizes4)
        {
            StringType s3(data.hello_world);
            RefType s4(data.hello_world);
            s3.replace(s3.cbegin() + 3, s3.cbegin() + 4, len, repl);
            s4.replace(s4.begin() + 3, s4.begin() + 4, len, repl);
            REQUIRE(s3.size() == s4.size());
            REQUIRE(Traits::compare(s3.data(), s4.data(), s3.size()) == 0);
        }

        // replace all
        s1.replace(s1.begin(), s1.end(), 0, repl);
        s2.replace(s2.begin(), s2.end(), 0, repl);
        REQUIRE(s1.size() == s2.size());
        REQUIRE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);
        REQUIRE(s1.empty());
        REQUIRE(s2.empty());
    }

    // replace(const_iterator first, const_iterator last, std::initializer_list<char_type> ilist);
    {
        // to work around some strange MSVC issues with std::initializer_list, we avoid using
        // TestData
        TestReplaceInitializerList<CharType, StringType> t;
        t.run();
    }

    // replace(size_type pos, size_type count, const String&);
    {
        StringType s1(data.hello_world);
        RefType s2(data.hello_world);
        // we use basic_string here
        RefType repl;

        // same length
        repl.assign(data.blablabla, 3);
        s1.replace(0, 3, repl);
        s2.replace(0, 3, repl);
        REQUIRE(s1.size() == s2.size());
        REQUIRE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);

        // longer (forcing a reallocation for Password, but shorter than the max. array)
        repl.assign(32, data.blablabla[0]);
        s1.replace(0, 3, repl);
        s2.replace(0, 3, repl);
        REQUIRE(s1.size() == s2.size());
        REQUIRE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);

        // shorter
        repl.assign(1, data.blablabla[0]);
        s1.replace(1, 4, repl);
        s2.replace(1, 4, repl);
        REQUIRE(s1.size() == s2.size());
        REQUIRE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);

        // replace all
        repl.clear();
        s1.replace(0, s1.size(), repl);
        s2.replace(0, s2.size(), repl);
        REQUIRE(s1.size() == s2.size());
        REQUIRE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);
        REQUIRE(s1.empty());
        REQUIRE(s2.empty());

        // exceptions
        s1 = data.hello_world;
        repl = data.blablabla;
        // (1) invalid pos raises out_of_range
        REQUIRE_THROWS_AS(s1.replace(s1.size(), s1.size() + 3, repl), std::out_of_range);
        // (2) in our class, an invalid count does the same
        REQUIRE_THROWS_AS(s1.replace(0, s1.size() + 1, repl), std::out_of_range);
    }

    // replace(const_iterator first, const_iterator last, const String&);
    {
        StringType s1(data.hello_world);
        RefType s2(data.hello_world);
        // we use basic_string here
        RefType repl;

        // same length
        repl.assign(data.blablabla, 3);
        s1.replace(s1.cbegin(), s1.cbegin() + 3, repl);
        s2.replace(s2.begin(), s2.begin() + 3, repl);
        REQUIRE(s1.size() == s2.size());
        REQUIRE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);

        // longer (forcing a reallocation for Password, but shorter than the max. array)
        repl.assign(32, data.blablabla[0]);
        s1.replace(s1.cbegin(), s1.cbegin() + 3, repl);
        s2.replace(s2.begin(), s2.begin() + 3, repl);
        REQUIRE(s1.size() == s2.size());
        REQUIRE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);

        // shorter
        repl.assign(1, data.blablabla[0]);
        s1.replace(s1.cbegin() + 1, s1.cbegin() + 4, repl);
        s2.replace(s2.begin() + 1, s2.begin() + 4, repl);
        REQUIRE(s1.size() == s2.size());
        REQUIRE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);

        // replace all
        repl.clear();
        s1.replace(s1.begin(), s1.end(), repl);
        s2.replace(s2.begin(), s2.end(), repl);
        REQUIRE(s1.size() == s2.size());
        REQUIRE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);
        REQUIRE(s1.empty());
        REQUIRE(s2.empty());
    }

    // replace(size_type pos, size_type count, const String&, size_type pos2, size_type count2 =
    // npos);
    {
        // again, using basic_string
        RefType repl;

        // same length
        repl.assign(data.blablabla, 3);
        std::array<size_t, 2> sizes1{ 3, npos };
        for (auto len : sizes1)
        {
            StringType s1(data.hello_world);
            RefType s2(data.hello_world);
            s1.replace(0, 3, repl, 0, len);
            s2.replace(0, 3, repl, 0, len);
            REQUIRE(s1.size() == s2.size());
            REQUIRE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);
        }

        // longer (forcing a reallocation for Password, but shorter than the max. array)
        repl.assign(32, data.blablabla[0]);
        std::array<size_t, 2> sizes2{ 32, npos };
        for (auto len : sizes2)
        {
            StringType s1(data.hello_world);
            RefType s2(data.hello_world);
            s1.replace(0, 3, repl, 0, len);
            s2.replace(0, 3, repl, 0, len);
            REQUIRE(s1.size() == s2.size());
            REQUIRE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);
        }

        // shorter
        repl.assign(1, data.blablabla[0]);
        std::array<size_t, 2> sizes3{ 1, npos };
        for (auto len : sizes3)
        {
            StringType s1(data.hello_world);
            RefType s2(data.hello_world);
            s1.replace(0, 3, repl, 0, len);
            s2.replace(0, 3, repl, 0, len);
            REQUIRE(s1.size() == s2.size());
            REQUIRE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);
        }

        // various lengths, but with pos2 != 0
        repl = data.blablabla;
        std::array<size_t, 2> sizes4{ 3, npos };
        for (auto len : sizes4)
        {
            StringType s1(data.hello_world);
            RefType s2(data.hello_world);
            s1.replace(0, 3, repl, 3, len);
            s2.replace(0, 3, repl, 3, len);
            REQUIRE(s1.size() == s2.size());
            REQUIRE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);
        }

        // replace all
        repl.clear();
        std::array<size_t, 2> sizes5{ 0, npos };
        for (auto len : sizes5)
        {
            StringType s1(data.hello_world);
            RefType s2(data.hello_world);
            s1.replace(0, s1.size(), repl, 0, len);
            s2.replace(0, s2.size(), repl, 0, len);
            REQUIRE(s1.size() == s2.size());
            REQUIRE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);
            REQUIRE(s1.empty());
            REQUIRE(s2.empty());
        }

        // exceptions: verify that they are thrown and that the string doesn't change
        {
            StringType s3(data.hello_world);
            // (1) invalid pos raises out_of_range
            REQUIRE_THROWS_AS(s3.replace(s3.size(), s3.size() + 3, repl, 0, repl.size()),
                              std::out_of_range);
            REQUIRE(s3 == data.hello_world);
            // (2) in our class, an invalid count does the same
            REQUIRE_THROWS_AS(s3.replace(0, s3.size() + 1, repl, 0, repl.size()),
                              std::out_of_range);
            REQUIRE(s3 == data.hello_world);
        }
    }
}
TEST_CASE_TEMPLATE_APPLY(StringBase_replace, StringBaseTestTypes);
