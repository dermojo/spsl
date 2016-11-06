/**
 * @file    Special Purpose Strings Library: example.cpp
 * @author  Daniel Evers
 * @brief   Simple usage examples
 * @license MIT
 */

#include <spsl.hpp>

int main(int, const char**)
{
    spsl::ArrayString<64> a1("I'm a stack-based string and can store 64 characters + NUL");
    spsl::ArrayStringW<32> a2(L"Wide characters are ok, too.");
    spsl::PasswordString pw1("I'm a secret!");
    spsl::PasswordStringW pw2(L"Dream on...");
    return 0;
}
