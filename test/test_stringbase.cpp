/**
 * @author	Daniel Evers
 * @brief	StringBase unit tests: various functions
 * @license MIT
 */

#include <sstream>

#include "catch.hpp"

#include "spsl.hpp"
#include "testdata.hpp"
#include "tests.hpp"


/* insert functions */
TEMPLATE_LIST_TEST_CASE("StringBase insert", "[string_base]", StringBaseTestTypes)
{
    using StringType = TestType;
    using StorageType = typename StringType::storage_type;
    using CharType = typename StorageType::char_type;
    using Traits = typename StorageType::traits_type;
    const TestData<CharType> data;

    using RefType = std::basic_string<CharType>;

    // this_type& insert(size_type index, size_type count, char_type ch)
    {
        StringType s1(data.hello_world);
        RefType s2(data.hello_world);

        s1.insert(3, 13, data.hello_world[0]);
        s2.insert(3, 13, data.hello_world[0]);
        REQUIRE(s1.size() == s2.size());
        REQUIRE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);
    }
    // this_type& insert(size_type index, const char_type* s)
    {
        StringType s1(data.hello_world);
        RefType s2(data.hello_world);

        s1.insert(0, data.blablabla);
        s2.insert(0, data.blablabla);
        REQUIRE(s1.size() == s2.size());
        REQUIRE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);
    }
    // this_type& insert(size_type index, const char_type* s, size_type count)
    {
        StringType s1(data.hello_world);
        RefType s2(data.hello_world);

        s1.insert(s1.size() - 2, data.blablabla, data.blablabla_len - 1);
        s2.insert(s2.size() - 2, data.blablabla, data.blablabla_len - 1);
        REQUIRE(s1.size() == s2.size());
        REQUIRE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);
    }
    // this_type& insert(size_type index, const StringClass& s)
    {
        StringType s1(data.hello_world);
        RefType s2(data.hello_world);
        // we use basic_string here
        const RefType ins(data.blablabla);

        s1.insert(1, ins);
        s2.insert(1, ins);
        REQUIRE(s1.size() == s2.size());
        REQUIRE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);
    }
    // this_type& insert(size_type, const StringClass&, size_type, size_type count)
    {
        StringType s1(data.hello_world);
        RefType s2(data.hello_world);
        // we use basic_string here
        const RefType ins(data.blablabla);

        s1.insert(1, ins, 3, StringType::npos);
        s2.insert(1, ins, 3, RefType::npos);
        REQUIRE(s1.size() == s2.size());
        REQUIRE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);
    }

    // iterator insert(const_iterator pos, char_type ch)
    {
        StringType s1(data.hello_world);
        RefType s2(data.hello_world);

        s1.insert(s1.begin() + 5, data.hello_world[0]);
        s2.insert(s2.begin() + 5, data.hello_world[0]);
        REQUIRE(s1.size() == s2.size());
        REQUIRE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);
    }
    // iterator insert(const_iterator pos, size_type count, char_type ch)
    {
        StringType s1(data.hello_world);
        RefType s2(data.hello_world);

        s1.insert(s1.begin(), 9, data.hello_world[0]);
        s2.insert(s2.begin(), 9, data.hello_world[0]);
        REQUIRE(s1.size() == s2.size());
        REQUIRE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);
    }
    // iterator insert(const_iterator pos, InputIt first, InputIt last)
    {
        StringType s1(data.hello_world);
        RefType s2(data.hello_world);
        RefType ins(data.blablabla);

        // "normal" iterator
        s1.insert(s1.begin() + 2, ins.begin(), ins.end());
        s2.insert(s2.begin() + 2, ins.begin(), ins.end());
        REQUIRE(s1.size() == s2.size());
        REQUIRE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);

        // const_iterator
        s1.insert(s1.begin() + 2, ins.cbegin(), ins.cbegin() + 6);
        s2.insert(s2.begin() + 2, ins.cbegin(), ins.cbegin() + 6);
        REQUIRE(s1.size() == s2.size());
        REQUIRE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);

        // reverse_iterator
        s1.insert(s1.begin() + 2, ins.rbegin(), ins.rend());
        s2.insert(s2.begin() + 2, ins.rbegin(), ins.rend());
        REQUIRE(s1.size() == s2.size());
        REQUIRE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);
    }
    // iterator insert(const_iterator pos, std::initializer_list<char_type> ilist)
    {
        StringType s1(data.hello_world);
        RefType s2(data.hello_world);

        s1.insert(s1.begin(), data.initializerList2());
        s2.insert(s2.begin(), data.initializerList2());
        REQUIRE(s1.size() == s2.size());
        REQUIRE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);
    }
}


// simple type trait to test for streamability
template <typename S, typename T>
class is_streamable
{
    template <typename SS, typename TT>
    static auto test(int) -> decltype(std::declval<SS&>() << std::declval<TT>(), std::true_type());

    template <typename, typename>
    static auto test(...) -> std::false_type;

public:
    static const bool value = decltype(test<S, T>(0))::value;
};

/* operator<< */
TEMPLATE_LIST_TEST_CASE("StringBase output stream", "[string_base]", StringBaseStreamableTypes)
{
    using StringType = TestType;
    using StorageType = typename StringType::storage_type;
    using CharType = typename StorageType::char_type;

    const TestData<CharType> data;

    const StringType s(data.hello_world);
    const std::basic_string<CharType> ref(data.hello_world);

    // using a string stream for testing
    using StreamType = std::basic_stringstream<CharType>;
    static_assert(is_streamable<StreamType, StringType>::value,
                  "expected this type to be streamable!");
    StreamType outputStream;
    outputStream << s;
    REQUIRE(outputStream.str() == ref);
}

TEMPLATE_LIST_TEST_CASE("StringBase non-streamable", "[string_base]", StringBaseNonStreamableTypes)
{
    using StringType = TestType;
    using StorageType = typename StringType::storage_type;
    using CharType = typename StorageType::char_type;
    using StreamType = std::basic_stringstream<CharType>;
    static_assert(!is_streamable<StreamType, StringType>::value,
                  "expected this type to be non-streamable!");
}
