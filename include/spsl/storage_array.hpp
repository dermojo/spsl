/**
 * @file	Special Purpose Strings Library: storage_array.hpp
 * @author  Daniel Evers
 * @brief	Storage implementation based on std::array
 * @license MIT
 */

#ifndef SPSL_STORAGE_ARRAY_HPP_
#define SPSL_STORAGE_ARRAY_HPP_

#include <array>
#include <limits>
#include <stdexcept>
#include <string> // for traits

#include "spsl/compat.hpp"

#ifdef _MSC_VER
// disable: "warning C4127: conditional expression is constant" (this is intentional...)
#pragma warning(push)
#pragma warning(disable : 4127)
#endif

namespace spsl
{

/**
 * Storage implementation based on a fixed-size array. It's intended to replace "raw" C character
 * arrays with a container that is almost as lightweight.
 *
 * Note that @c MaxSize is the upper limit for the number of characters, not including the
 * terminating NUL.
 *
 * This storage implementation silently ignores string truncation unless @¢ ThrowOnTruncate is
 * set to @c true.
 */
template <typename CharType, std::size_t MaxSize, bool ThrowOnTruncate = false>
class StorageArray
{
public:
    using size_type = std::size_t;
    using difference_type = ssize_t;
    using char_type = CharType;
    /// simple alias for the typing impaired :)
    using this_type = StorageArray<char_type, MaxSize, ThrowOnTruncate>;
    using traits_type = typename std::char_traits<char_type>;

    static constexpr char_type nul = static_cast<char_type>(0);

    // size information functions
    constexpr static size_type max_size() { return MaxSize; }
    constexpr static size_type capacity() { return MaxSize; }
    size_type length() const { return m_length; }
    size_type size() const { return m_length; }
    bool empty() const { return m_length == 0; }
    void reserve(size_type new_cap = 0) noexcept(!ThrowOnTruncate)
    {
        if (new_cap > max_size() && ThrowOnTruncate)
            throw std::length_error("requested capacity exceeds maximum");
    }
    // no-op - there is nothing to shrink
    void shrink_to_fit() {}

    inline size_type capacity_left() const { return max_size() - size(); }

    // default constructor
    StorageArray() : m_length(0), m_buffer() { m_buffer[0] = nul; }

    // implement copy & move
    StorageArray(const this_type& other) noexcept : StorageArray()
    {
        // note: assign() won't throw because 'other' has the same max. length
        assign(other.data(), other.size());
    }
    StorageArray(this_type&& other) noexcept : StorageArray()
    {
        // swapping is more expensive than this...
        // note: assign() won't throw because 'other' has the same max. length
        assign(other.data(), other.size());
        other.clear();
    }

    StorageArray& operator=(const this_type& other) noexcept
    {
        // note: assign() won't throw because 'other' has the same max. length and must be valid
        assign(other.data(), other.size());
        return *this;
    }
    StorageArray& operator=(this_type&& other) noexcept
    {
        // swapping is more expensive than this...
        // note: assign() won't throw because 'other' has the same max. length
        assign(other.data(), other.size());
        other.clear();
        return *this;
    }

    // default destructor - nothing to do
    ~StorageArray() = default;


    // buffer access functions

    char_type* data() { return m_buffer.data(); }
    const char_type* data() const { return m_buffer.data(); }

    char_type& operator[](size_type pos) { return m_buffer[pos]; }
    const char_type& operator[](size_type pos) const { return m_buffer[pos]; }

    void assign(const char_type* s, size_type n) noexcept(!ThrowOnTruncate)
    {
        size_type len = std::min(n, max_size());
        if (len != n && ThrowOnTruncate)
            throw std::length_error("string length exceeds maximum capacity");

        traits_type::copy(m_buffer.data(), s, len);
        m_length = len;
        m_buffer[m_length] = nul;
    }
    void assign(size_type count, char_type ch) noexcept(!ThrowOnTruncate)
    {
        size_type len = std::min(count, max_size());
        if (len != count && ThrowOnTruncate)
            throw std::length_error("string length exceeds maximum capacity");

        traits_type::assign(m_buffer.data(), len, ch);
        m_length = len;
        m_buffer[m_length] = nul;
    }

    template <typename InputIt>
    void assign(InputIt first, InputIt last) noexcept(!ThrowOnTruncate)
    {
        // is there enough space?
        const size_type n = std::distance(first, last);
        const size_type len = std::min(n, max_size());
        if (len != n && ThrowOnTruncate)
            throw std::length_error("string size exceeds maximum buffer capacity");

        m_length = 0;
        for (size_type i = 0; i < len && first != last; ++i)
        {
            m_buffer[m_length++] = *first++;
        }
        m_buffer[m_length] = nul;
    }

    void clear()
    {
        m_buffer[0] = nul;
        m_length = 0;
    }
    void push_back(char_type c) noexcept(!ThrowOnTruncate)
    {
        if (capacity_left())
        {
            m_buffer[m_length++] = c;
            m_buffer[m_length] = nul;
        }
        else if (ThrowOnTruncate)
        {
            throw std::length_error("string size exceeds maximum buffer capacity");
        }
    }
    void pop_back()
    {
        // the standard leaves it as "undefined" if m_length == 0, but we'll just keep it sane
        if (m_length != 0)
            m_buffer[--m_length] = nul;
    }

    void insert(size_type index, size_type count, char_type ch) noexcept(!ThrowOnTruncate)
    {
        if (index > size())
            throw std::out_of_range("index out of range");

        // use a temporary string - much easier
        this_type tmp;
        tmp.assign(data(), index);
        tmp.append(count, ch);
        tmp.append(data() + index, size() - index);

        *this = tmp;
    }

    void insert(size_type index, const char_type* s, size_type n) noexcept(!ThrowOnTruncate)
    {
        if (index > size())
            throw std::out_of_range("index out of range");

        // use a temporary string - much easier
        this_type tmp;
        tmp.assign(data(), index);
        tmp.append(s, n);
        tmp.append(data() + index, size() - index);

        *this = tmp;
    }

    template <typename InputIt>
    void insert(size_type index, InputIt first, InputIt last) noexcept(!ThrowOnTruncate)
    {
        if (index > size())
            throw std::out_of_range("index out of range");

        // use a temporary string - much easier
        this_type tmp;
        tmp.assign(data(), index);
        tmp.append(first, last);
        tmp.append(data() + index, size() - index);

        *this = tmp;
    }

    void erase(size_type index, size_type count) noexcept
    {
        // move all following characters here, including the NUL terminator
        const size_type n = size() - index - count + 1;
        traits_type::move(m_buffer.data() + index, m_buffer.data() + index + count, n);
        m_length -= count;
    }


    void append(size_type count, char_type ch) noexcept(!ThrowOnTruncate)
    {
        // is there enough space?
        size_type appendCount = std::min(count, capacity_left());
        if (appendCount != count && ThrowOnTruncate)
            throw std::length_error("string size exceeds maximum buffer capacity");

        traits_type::assign(m_buffer.data() + m_length, appendCount, ch);
        m_length += appendCount;
        m_buffer[m_length] = nul;
    }

    void append(const char_type* s, size_type n) noexcept(!ThrowOnTruncate)
    {
        // is there enough space?
        const size_type appendCount = std::min(n, capacity_left());
        if (appendCount != n && ThrowOnTruncate)
            throw std::length_error("string size exceeds maximum buffer capacity");

        traits_type::copy(m_buffer.data() + m_length, s, appendCount);
        m_length += appendCount;
        m_buffer[m_length] = nul;
    }

    template <typename InputIt>
    void append(InputIt first, InputIt last) noexcept(!ThrowOnTruncate)
    {
        // is there enough space?
        const size_type n = std::distance(first, last);
        const size_type appendCount = std::min(n, capacity_left());
        if (appendCount != n && ThrowOnTruncate)
            throw std::length_error("string size exceeds maximum buffer capacity");

        for (size_type i = 0; i < appendCount && first != last; ++i)
        {
            m_buffer[m_length++] = *first++;
        }
        m_buffer[m_length] = nul;
    }

    void resize(size_type count, char_type ch) noexcept(!ThrowOnTruncate)
    {
        if (count < size())
        {
            m_length = count;
        }
        else if (count > size())
        {
            const size_type n = std::min(count, max_size());
            if (n != count && ThrowOnTruncate)
                throw std::length_error("string size exceeds maximum buffer capacity");

            traits_type::assign(m_buffer.data() + m_length, n, ch);
            m_length = n;
        }
        m_buffer[m_length] = nul;
    }

    void replace(size_type pos, size_type count, const char_type* cstr,
                 size_type count2) noexcept(!ThrowOnTruncate)
    {
        // simple implementation (avoid a lot of memmove's): create a new string and swap
        // => This is ok because there is no heap allocation, making this actually fast.
        //    Also, this will handle string truncation properly and is exception safe.
        this_type tmp;

        // (1) copy the part before 'pos'
        if (pos != 0)
            tmp.assign(data(), pos);

        // (2) append the replacement string
        tmp.append(cstr, count2);

        // (3) append the rest after 'pos + count'
        size_type rest = pos + count;
        if (rest < size())
            tmp.append(data() + rest, size() - rest);

        // (4) swap/move
        *this = std::move(tmp);
    }

    void replace(size_type pos, size_type count, size_type count2,
                 char_type ch) noexcept(!ThrowOnTruncate)
    {
        // => same implementation as above
        this_type tmp;

        // (1) copy the part before 'pos'
        if (pos != 0)
            tmp.assign(data(), pos);

        // (2) append the replacement string
        tmp.append(count2, ch);

        // (3) append the rest after 'pos + count'
        size_type rest = pos + count;
        if (rest < size())
            tmp.append(data() + rest, size() - rest);

        // (4) swap/move
        *this = std::move(tmp);
    }

    template <class InputIt>
    void replace(size_type pos, size_type count, InputIt first, InputIt last)
    {
        // => same implementation as above
        this_type tmp;

        // (1) copy the part before 'pos'
        if (pos != 0)
            tmp.assign(data(), pos);

        // (2) append the replacement string
        tmp.append(first, last);

        // (3) append the rest after 'pos + count'
        size_type rest = pos + count;
        if (rest < size())
            tmp.append(data() + rest, size() - rest);

        // (4) swap/move
        *this = std::move(tmp);
    }

    void swap(this_type& other) { std::swap(*this, other); }

protected:
    /// number of bytes (*not* characters) in the buffer, not including the terminating NUL
    size_type m_length;
    /// the underlying buffer: an array that can hold MaxSize characters (+ terminating NUL)
    std::array<char_type, MaxSize + 1> m_buffer;
};
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif /* SPSL_STORAGE_ARRAY_HPP_ */
