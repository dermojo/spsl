/**
 * @file	Special Purpose Strings Library: test_stringbase.cpp
 * @author	Daniel Evers
 * @brief	StringBase unit tests: various functions
 * @license MIT
 */

#include <gtest/gtest.h>
#include <sstream>

#include "spsl.hpp"
#include "testdata.hpp"
#include "tests.hpp"


/* insert functions */
TYPED_TEST(StringBaseTest, InsertFunctions)
{
    using StringType = TypeParam; // gtest specific
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
        ASSERT_EQ(s1.size(), s2.size());
        ASSERT_TRUE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0)
#ifdef ENABLE_HEXDUMP
          << hexdump(s1.data(), s1.size()) << "\n"
          << hexdump(s2.data(), s2.size())
#endif
          ;
    }
    // this_type& insert(size_type index, const char_type* s)
    {
        StringType s1(data.hello_world);
        RefType s2(data.hello_world);

        s1.insert(0, data.blablabla);
        s2.insert(0, data.blablabla);
        ASSERT_EQ(s1.size(), s2.size());
        ASSERT_TRUE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);
    }
    // this_type& insert(size_type index, const char_type* s, size_type count)
    {
        StringType s1(data.hello_world);
        RefType s2(data.hello_world);

        s1.insert(s1.size() - 2, data.blablabla, data.blablabla_len - 1);
        s2.insert(s2.size() - 2, data.blablabla, data.blablabla_len - 1);
        ASSERT_EQ(s1.size(), s2.size());
        ASSERT_TRUE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);
    }
    // this_type& insert(size_type index, const StringClass& s)
    {
        StringType s1(data.hello_world);
        RefType s2(data.hello_world);
        // we use basic_string here
        const RefType ins(data.blablabla);

        s1.insert(1, ins);
        s2.insert(1, ins);
        ASSERT_EQ(s1.size(), s2.size());
        ASSERT_TRUE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);
    }
    // this_type& insert(size_type, const StringClass&, size_type, size_type count)
    {
        StringType s1(data.hello_world);
        RefType s2(data.hello_world);
        // we use basic_string here
        const RefType ins(data.blablabla);

        s1.insert(1, ins, 3, StringType::npos);
        s2.insert(1, ins, 3, RefType::npos);
        ASSERT_EQ(s1.size(), s2.size());
        ASSERT_TRUE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);
    }

    // iterator insert(const_iterator pos, char_type ch)
    {
        StringType s1(data.hello_world);
        RefType s2(data.hello_world);

        s1.insert(s1.begin() + 5, data.hello_world[0]);
        s2.insert(s2.begin() + 5, data.hello_world[0]);
        ASSERT_EQ(s1.size(), s2.size());
        ASSERT_TRUE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);
    }
    // iterator insert(const_iterator pos, size_type count, char_type ch)
    {
        StringType s1(data.hello_world);
        RefType s2(data.hello_world);

        s1.insert(s1.begin(), 9, data.hello_world[0]);
        s2.insert(s2.begin(), 9, data.hello_world[0]);
        ASSERT_EQ(s1.size(), s2.size());
        ASSERT_TRUE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);
    }
    // iterator insert(const_iterator pos, InputIt first, InputIt last)
    {
        StringType s1(data.hello_world);
        RefType s2(data.hello_world);
        RefType ins(data.blablabla);

        // "normal" iterator
        s1.insert(s1.begin() + 2, ins.begin(), ins.end());
        s2.insert(s2.begin() + 2, ins.begin(), ins.end());
        ASSERT_EQ(s1.size(), s2.size());
        ASSERT_TRUE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);

        // const_iterator
        s1.insert(s1.begin() + 2, ins.cbegin(), ins.cbegin() + 6);
        s2.insert(s2.begin() + 2, ins.cbegin(), ins.cbegin() + 6);
        ASSERT_EQ(s1.size(), s2.size());
        ASSERT_TRUE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);

        // reverse_iterator
        s1.insert(s1.begin() + 2, ins.rbegin(), ins.rend());
        s2.insert(s2.begin() + 2, ins.rbegin(), ins.rend());
        ASSERT_EQ(s1.size(), s2.size());
        ASSERT_TRUE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);
    }
    // iterator insert(const_iterator pos, std::initializer_list<char_type> ilist)
    {
        StringType s1(data.hello_world);
        RefType s2(data.hello_world);

        s1.insert(s1.begin(), data.initializerList2());
        s2.insert(s2.begin(), data.initializerList2());
        ASSERT_EQ(s1.size(), s2.size());
        ASSERT_TRUE(Traits::compare(s1.data(), s2.data(), s1.size()) == 0);
    }
}

template <class StringType, class CharType>
void runStreamTests(StringType&&, CharType)
{
    const TestData<CharType> data;

    const StringType s(data.hello_world);
    const std::basic_string<CharType> ref(data.hello_world);

    // using a string stream for testing
    std::basic_stringstream<CharType> outputStream;
    outputStream << s;
    ASSERT_EQ(outputStream.str(), ref);
}

// disable for gsl::byte
template <class StringType>
void runStreamTests(StringType&&, gsl::byte)
{
}

/* operator<< */
TYPED_TEST(StringBaseTest, OutputStream)
{
    using StringType = TypeParam; // gtest specific
    using StorageType = typename StringType::storage_type;
    using CharType = typename StorageType::char_type;
    runStreamTests(StringType(), CharType());
}
