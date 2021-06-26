/**
 * @author  Daniel Evers
 * @brief   Common data for all unit tests
 * @license MIT
 */

#ifndef TESTDATA_HPP_
#define TESTDATA_HPP_

#include <cstddef>
#include <cstring>
#include <string>
#include <string_view>


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
    TestData() {}
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

/*
 * Some global operators that help dealing with std::byte in our tests.
 */

template <class CharType>
constexpr std::byte to_byte(CharType b)
{
    return static_cast<std::byte>(b);
}

/// used to declare literals
constexpr std::byte operator"" _b(char b)
{
    return to_byte(b);
}


#ifdef ENABLE_HEXDUMP
inline std::string hexdump(const void* ptr, size_t buflen)
{
    std::string buffer;
    const char* buf = reinterpret_cast<const char*>(ptr);
    char tmp[64];
    for (size_t i = 0; i < buflen; i += 16)
    {
        snprintf(tmp, sizeof(tmp), "%06zu: ", i);
        buffer += tmp;
        for (size_t j = 0; j < 16; j++)
        {
            if (i + j < buflen)
                snprintf(tmp, sizeof(tmp), "%02x ", buf[i + j]);
            else
                snprintf(tmp, sizeof(tmp), "   ");
            buffer += tmp;
        }
        buffer += " ";
        for (size_t j = 0; j < 16; j++)
        {
            if (i + j < buflen)
            {
                buffer += (isprint(buf[i + j]) ? buf[i + j] : '.');
            }
        }
        buffer += '\n';
    }
    return buffer;
}
#endif

#endif /* TESTDATA_HPP_ */
