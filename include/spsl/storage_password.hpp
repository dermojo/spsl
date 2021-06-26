/**
 * @file	Special Purpose Strings Library: storage_password.hpp
 * @author  Daniel Evers
 * @brief	Storage implementation that wipes memory before free'ing it - for sensitive data
 * @license MIT
 */

#ifndef SPSL_STORAGE_PASSWORD_HPP_
#define SPSL_STORAGE_PASSWORD_HPP_

#include <limits>
#include <stdexcept>
#include <string> // for traits

#include "spsl/pagealloc.hpp"
#include "spsl/type_traits.hpp"

namespace spsl
{

/**
 * Secure memset implementation that may not be optimized "away".
 * @param[in] ptr       pointer to the memory area to clear
 * @param[in] size      number of bytes to clear
 * @return @c ptr
 */
inline void* secure_memzero(void* ptr, size_t size) noexcept
{
    // Note: We are *not* using SecureZeroMemory() here because this requires us to include the
    // Windows headers and then all hell breaks loose...
    // Also, the implementation is pretty much the same :)

    // For more info, start here:
    // http://stackoverflow.com/questions/9973260/what-is-the-correct-way-to-clear-sensitive-data-from-memory-in-ios

    volatile char* p = reinterpret_cast<char*>(ptr);
    while (size--)
        *p++ = 0;
    return ptr;
}


/**
 * Storage implementation that wipes all memory before free'ing it. It's therefore usable for
 * passwords and other sensitive data that shouldn't be "left behind" when releasing memory
 * back to the OS.
 *
 * The allocation strategy is simple: We always allocate a multiple of the block size, assuming
 * that passwords and other sensitive data is relatively static. We rely on an allocator type,
 * which defaults to std::allocator, so that we can swap out the allocator in the unit tests.
 * Only the allocate(), deallocate() and max_size() member functions of the allocator type are used.
 */
template <typename CharType, std::size_t BlockSize = 128,
          typename Allocator = SensitiveSegmentAllocator<CharType>>
class StoragePassword : private Allocator
{
public:
    using size_type = std::size_t;
    using difference_type = ssize_t;
    using char_type = CharType;
    /// simple alias for the typing impaired :)
    using this_type = StoragePassword<char_type, BlockSize, Allocator>;
    using traits_type = typename std::char_traits<char_type>;
    using allocator = Allocator;

    static constexpr char_type nul = static_cast<char_type>(0);

    // size information functions
    size_type max_size() const
    {
        const allocator& a = *this;
        return a.max_size();
    }
    constexpr static size_type block_size() { return BlockSize; }
    size_type capacity() const { return _l.m_capacity; }
    size_type length() const { return _l.m_length; }
    size_type size() const { return _l.m_length; }
    bool empty() const { return _l.m_length == 0; }

    // access to the underlying allocator
    allocator& getAllocator() noexcept { return *this; }
    const allocator& getAllocator() const noexcept { return *this; }

    // note: This function is *not* constexpr to stay compatible with C++11
    static size_type _roundRequiredCapacityToBlockSize(size_type cap)
    {
        size_type numBlocks = cap / BlockSize;
        if (numBlocks * BlockSize < cap)
            ++numBlocks;
        return numBlocks * BlockSize;
    }

    void reserve(size_type new_cap = 0)
    {
        if (new_cap > max_size())
            throw std::length_error("requested capacity exceeds maximum");
        if (new_cap >= capacity())
        {
            // need to realloc: We explicitly allocate a new block, copy all data and wipe the old
            new_cap = _roundRequiredCapacityToBlockSize(new_cap + 1);

            // allocate a new buffer (the allocator will throw in case of error)
            allocator& a = *this;
            char_type* newbuf = a.allocate(new_cap);

            // copy existing data (note: all data must fit because we're only growing)
            if (size())
                traits_type::copy(newbuf, m_buffer, size() + 1);
            else
                newbuf[0] = nul;

            // wipe all existing data
            if (m_buffer != _b)
            {
                _wipe();
                a.deallocate(m_buffer, capacity());
            }

            // now replace our data
            m_buffer = newbuf;
            _l.m_capacity = new_cap;
        }
    }

    // get rid of unnecessarily allocated data
    void shrink_to_fit()
    {
        // quick & dirty implementation: create a copy of this string and swap...
        if ((empty() && capacity()) || (capacity() - (size() + 1) >= BlockSize))
        {
            this_type copy(*this);
            swap(copy);
        }
    }

    void _set_length(size_type n)
    {
        _l.m_length = n;
        traits_type::assign(m_buffer[n], nul);
    }

    // default constructor
    StoragePassword(const allocator& alloc = allocator()) noexcept : allocator(alloc), m_buffer(_b)
    {
        _l.m_capacity = 0;
        _set_length(0);
    }

    // implement copy & move
    StoragePassword(const this_type& other) : StoragePassword(other.getAllocator())
    {
        if (!other.empty())
            assign(other.data(), other.size());
    }
    StoragePassword(this_type&& other) noexcept : StoragePassword(other.getAllocator())
    {
        swap(other);
        other.clear();
    }

    StoragePassword& operator=(const this_type& other)
    {
        if (this != &other)
            assign(other.data(), other.size());
        return *this;
    }
    StoragePassword& operator=(this_type&& other) noexcept
    {
        swap(other);
        other.clear();
        return *this;
    }

    // default destructor: wipe & release
    ~StoragePassword()
    {
        if (m_buffer != _b)
        {
            clear();
            allocator& a = *this;
            a.deallocate(m_buffer, capacity());
        }
    }


    // buffer access functions

    char_type* data() { return m_buffer; }
    const char_type* data() const { return m_buffer; }

    char_type& operator[](size_type pos) { return data()[pos]; }
    const char_type& operator[](size_type pos) const { return data()[pos]; }

    void assign(const char_type* s, size_type n)
    {
        reserve(n);

        traits_type::copy(m_buffer, s, n);
        _set_length(n);
    }
    void assign(size_type count, char_type ch)
    {
        reserve(count);

        traits_type::assign(m_buffer, count, ch);
        _set_length(count);
    }

    template <typename InputIt, typename = checkInputIter<InputIt>>
    void assign(InputIt first, InputIt last)
    {
        clear();
        append(first, last);
    }

    void clear()
    {
        if (m_buffer != _b)
        {
            _wipe();
        }
        _l.m_length = 0;
    }
    void push_back(char_type c)
    {
        reserve(size() + 1);
        traits_type::assign(m_buffer[_l.m_length++], c);
        traits_type::assign(m_buffer[_l.m_length], nul);
    }
    void pop_back()
    {
        // the standard leaves it as "undefined" if size() == 0, but we'll just keep it sane
        if (size() != 0)
            _set_length(size() - 1);
    }

    void insert(size_type index, size_type count, char_type ch)
    {
        if (index > size())
            throw std::out_of_range("index out of range");

        reserve(size() + count);

        // move the existing data (including the terminating NUL)
        char_type* ptr = m_buffer + index;
        const size_type len = size() + 1 - index;
        traits_type::move(ptr + count, ptr, len);

        for (size_type i = 0; i < count; ++i)
            traits_type::assign(ptr[i], ch);

        _l.m_length += count;
    }

    void insert(size_type index, const char_type* s, size_type n)
    {
        if (index > size())
            throw std::out_of_range("index out of range");

        reserve(size() + n);

        // move the existing data (including the terminating NUL)
        char_type* ptr = m_buffer + index;
        const size_type len = size() + 1 - index;
        traits_type::move(ptr + n, ptr, len);
        traits_type::copy(ptr, s, n);

        _l.m_length += n;
    }

    template <typename InputIt, typename = checkInputIter<InputIt>>
    void insert(size_type index, InputIt first, InputIt last)
    {
        this_type tmp;
        tmp.assign(first, last);
        insert(index, tmp.data(), tmp.size());
    }


    void erase(size_type index, size_type count) noexcept
    {
        // move all following characters here
        const size_type n = size() - index - count;
        traits_type::move(data() + index, data() + index + count, n);

        _l.m_length -= count;
        // wipe the rest
        _wipe(_l.m_length, n);
    }


    void append(size_type count, char_type ch)
    {
        reserve(size() + count);
        traits_type::assign(m_buffer + size(), count, ch);
        _set_length(size() + count);
    }

    void append(const char_type* s, size_type n)
    {
        reserve(size() + n);
        traits_type::copy(m_buffer + size(), s, n);
        _set_length(size() + n);
    }

    template <typename InputIt, typename = checkInputIter<InputIt>>
    void append(InputIt first, InputIt last)
    {
        while (first != last)
        {
            push_back(*first);
            ++first;
        }
    }

    void replace(size_type pos, size_type count, const char_type* cstr, size_type count2)
    {
        // lazy implementation:
        //   - different length: build a new string and swap...
        //   - same length: overwrite the part to replace
        // This is *not* the most efficient implementation, but it's easy and exception safe :)
        // TODO: avoid reallocations, fix this

        if (count == count2)
        {
            traits_type::copy(data() + pos, cstr, count2);
        }
        else
        {
            this_type tmp;
            tmp.reserve(size() - count + count2);

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
    }

    void replace(size_type pos, size_type count, size_type count2, char_type ch)
    {
        // => same implementation as above
        // TODO: avoid reallocations, fix this

        if (count == count2)
        {
            traits_type::assign(data() + pos, count2, ch);
        }
        else
        {
            this_type tmp;
            tmp.reserve(size() - count + count2);

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
    }

    template <class InputIt>
    void replace(size_type pos, size_type count, InputIt first, InputIt last)
    {
        // => same implementation as above
        this_type tmp;
        tmp.assign(first, last);
        replace(pos, count, tmp.data(), tmp.size());
    }

    void resize(size_type count, char_type ch)
    {
        if (count < size())
        {
            // wipe the remaining content
            traits_type::assign(m_buffer + count, size() - count, nul);
        }
        else if (count > size())
        {
            reserve(count);
            traits_type::assign(m_buffer + size(), count, ch);
        }
        _set_length(count);
    }

    void swap(this_type& other) noexcept
    {
        std::swap(_l.m_length, other._l.m_length);
        std::swap(_l.m_capacity, other._l.m_capacity);
        // avoid swapping internal pointers...
        std::swap(m_buffer, other.m_buffer);
        if (other.m_buffer == _b)
            other.m_buffer = other._b;
        if (m_buffer == other._b)
            m_buffer = _b;
        std::swap(getAllocator(), other.getAllocator());
    }

protected:
    void _wipe(size_type index, size_type count) noexcept
    {
        secure_memzero(m_buffer + index, count * sizeof(char_type));
    }
    void _wipe() noexcept { _wipe(0, capacity()); }


private:
    // we store length + capacity information in a union that we also use as empty string
    // representation (assumption: m_buffer == _b => m_length == m_capacity == 0)
    struct SizeInfo
    {
        /// number of bytes (*not* characters) in the buffer, not including the terminating NUL
        size_type m_length;
        /// number of bytes (*not* characters) allocated
        size_type m_capacity;
    };
    union
    {
        SizeInfo _l;
        // actually _b[1] is sufficient, but triggers overflow warnings in GCC
        char_type _b[sizeof(SizeInfo)];
    };

    /// the underlying buffer
    char_type* m_buffer;
};
} // namespace spsl


#endif /* SPSL_STORAGE_PASSWORD_HPP_ */
