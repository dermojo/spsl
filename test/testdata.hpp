/**
 * @file    Special Purpose Strings Library: testdata.hpp
 * @author  Daniel Evers
 * @brief   Common data for all unit tests
 * @license MIT
 */

#ifndef TESTDATA_HPP_
#define TESTDATA_HPP_

#include <cstddef>

// enable basic_string_view tests if available (according to http://stackoverflow.com/a/7132549)
#if __cplusplus > 201402L
#define TEST_STRING_VIEW

#include <experimental/string_view>

#endif


// define test strings for every character type
template <typename CharType>
struct TestData
{
};

template <>
struct TestData<char>
{
    TestData() = default;
    ~TestData() = default;
    // not needed
    TestData(const TestData&) = delete;
    TestData(TestData&&) = delete;
    TestData& operator=(const TestData&) = delete;
    TestData& operator=(TestData&&) = delete;

    const char* const hello_world = "Hello World!";
    const size_t hello_world_len = 12;
    const char* const blablabla = "blablabla";
    const size_t blablabla_len = 9;
    const char* const empty = "";

    std::initializer_list<char> initializerList() const
    {
        static const std::initializer_list<char> list{ 'T', 'h', 'i', 's', ' ', 'i', 's', ' ', 'a',
                                                       'n', ' ', 'a', 's', 's', 'i', 'g', 'n', 'm',
                                                       'e', 'n', 't', ' ', 't', 'e', 's', 't' };
        return list;
    }
    std::initializer_list<char> initializerList2() const
    {
        static const std::initializer_list<char> list{ 'T', 'e', 's', 't' };
        return list;
    }
};

template <>
struct TestData<wchar_t>
{
    TestData() = default;
    ~TestData() = default;
    // not needed
    TestData(const TestData&) = delete;
    TestData(TestData&&) = delete;
    TestData& operator=(const TestData&) = delete;
    TestData& operator=(TestData&&) = delete;

    const wchar_t* const hello_world = L"Hello World!";
    const size_t hello_world_len = 12;
    const wchar_t* const blablabla = L"blablabla";
    const size_t blablabla_len = 9;
    const wchar_t* const empty = L"";

    std::initializer_list<wchar_t> initializerList() const
    {
        static const std::initializer_list<wchar_t> list{ L'T', L'h', L'i', L's', L' ', L'i', L's',
                                                          L' ', L'a', L'n', L' ', L'a', L's', L's',
                                                          L'i', L'g', L'n', L'm', L'e', L'n', L't',
                                                          L' ', L't', L'e', L's', L't' };
        return list;
    }
    std::initializer_list<wchar_t> initializerList2() const
    {
        static const std::initializer_list<wchar_t> list{ L'T', L'e', L's', L't' };
        return list;
    }
};

#endif /* TESTDATA_HPP_ */
