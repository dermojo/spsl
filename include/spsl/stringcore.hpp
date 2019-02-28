/**
 * @file	Special Purpose Strings Library: stringcore.hpp
 * @author  Daniel Evers
 * @brief	Core string class functionality
 * @license MIT
 */

#ifndef SPSL_STRINGCORE_HPP_
#define SPSL_STRINGCORE_HPP_

#include <iterator>
#include <string>
#include <utility>

#include "spsl/hash.hpp"
#include "spsl/type_traits.hpp"

namespace spsl
{

/**
 * This class implements core string functionality, such as assignment and access functions.
 * Additional functionality (erase, replace, ...) may not be needed in some cases and is therefore
 * not contained here. See StringBase if you need those.
 *
 * The API mostly resembles std::basic_string, so there is not much documentation (needed).
 * The only difference: We're not using "allocators", but "storage types".
 */
template <typename StorageType>
class StringCore
{
public:
    /// alias for the typing impaired :)
    using this_type = StringCore<StorageType>;

    using storage_type = StorageType;
    using char_type = typename storage_type::char_type;
    using size_type = typename storage_type::size_type;
    using difference_type = typename storage_type::difference_type;
    using traits_type = typename storage_type::traits_type;
    // these types are required by the standard...
    using value_type = char_type;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    // well, somewhat...
    using allocator_type = storage_type;

    static constexpr size_type npos = static_cast<size_type>(-1);
    static constexpr char_type nul() { return char_type(); }


    // TODO For now, we use plain pointers. If this isn't sufficient, we need to roll our own...
    using iterator = pointer;
    using const_iterator = const_pointer;
    using const_reverse_iterator = typename std::reverse_iterator<const_iterator>;
    using reverse_iterator = typename std::reverse_iterator<iterator>;


    /* ************************************ CONSTRUCTORS ************************************ */

    StringCore() : m_storage() {}
    StringCore(const char_type* s) : m_storage() { assign(s); }
    StringCore(const char_type* s, size_type n) : m_storage() { assign(s, n); }
    StringCore(size_type numRepeat, char_type ch) : m_storage()
    {
        m_storage.insert(0, numRepeat, ch);
    }
    StringCore(std::initializer_list<char_type> init) : m_storage()
    {
        assign(init.begin(), init.size());
    }
    template <class InputIt, typename = checkInputIter<InputIt>>
    StringCore(InputIt first, InputIt last) : m_storage()
    {
        assign(first, last);
    }

    // default destructor, move and copy
    ~StringCore() = default;
    StringCore(const this_type&) = default;
    StringCore(this_type&& rhs) noexcept : StringCore() { this->swap(rhs); }

    /// construct from another storage instance
    explicit StringCore(const storage_type& storage) : m_storage(storage) {}

    /// construct from another string-like container (may even be a vector...)
    template <typename StringClass, typename std::enable_if<is_compatible_string<
                                      char_type, size_type, StringClass>::value>::type* = nullptr>
    explicit StringCore(const StringClass& s) : m_storage()
    {
        assign(s.data(), s.size());
    }


    /* ********************************** ASSIGNMENT OPERATORS ********************************** */

    this_type& operator=(const char_type* s) { return assign(s); }
    this_type& operator=(char_type c) { return assign(&c, 1); }
    this_type& operator=(std::initializer_list<char_type> init)
    {
        return assign(init.begin(), init.size());
    }

    // default copy & move
    this_type& operator=(const this_type&) = default;
    this_type& operator=(this_type&& rhs) noexcept
    {
        this->swap(rhs);
        return *this;
    }

    /// allow assignment from another string-like container (may even be a vector...)
    template <typename StringClass, typename std::enable_if<is_compatible_string<
                                      char_type, size_type, StringClass>::value>::type* = nullptr>
    this_type& operator=(const StringClass& s)
    {
        return assign(s.data(), s.size());
    }


    /* ********************************** ASSIGNMENT FUNCTIONS ********************************** */

    this_type& assign(size_type count, char_type c)
    {
        m_storage.assign(count, c);
        return *this;
    }

    this_type& assign(const char_type* s, size_type n)
    {
        m_storage.assign(s, n);
        return *this;
    }

    this_type& assign(const char_type* s) { return assign(s, traits_type::length(s)); }
    this_type& assign(std::initializer_list<char_type> init)
    {
        return assign(init.begin(), init.size());
    }

    template <typename InputIt, typename = checkInputIter<InputIt>>
    this_type& assign(InputIt first, InputIt last)
    {
        m_storage.assign(first, last);
        return *this;
    }

    // copy & move variants
    this_type& assign(const this_type& s) { return *this = s; }
    this_type& assign(this_type&& s) { return *this = std::move(s); }

    /// allow assignment from another string-like container or view (may even be a vector...)
    template <typename StringClass, typename std::enable_if<is_compatible_string<
                                      char_type, size_type, StringClass>::value>::type* = nullptr>
    this_type& assign(const StringClass& s)
    {
        return assign(s.data(), s.size());
    }

    /// same, but with pos and count
    template <typename StringClass, typename std::enable_if<is_compatible_string<
                                      char_type, size_type, StringClass>::value>::type* = nullptr>
    this_type& assign(const StringClass& s, size_type pos, size_type count = npos)
    {
        if (pos > s.size())
            throw std::out_of_range("pos > size()");
        return assign(s.data() + pos, (count == npos ? s.size() - pos : count));
    }


    /* ********************************** ACCESS FUNCTIONS ********************************** */

    /// @return a pointer to the internal C-string (NUL terminated)
    const char_type* c_str() const { return m_storage.data(); }
    const char_type* data() const { return m_storage.data(); }
    char_type* data() { return m_storage.data(); }

    char_type& operator[](size_type pos) { return m_storage[pos]; }
    const char_type& operator[](size_type pos) const { return m_storage[pos]; }
    char_type& at(size_type pos)
    {
        if (pos >= size())
            throw std::out_of_range("pos >= size()");
        return m_storage[pos];
    }
    const char_type& at(size_type pos) const
    {
        if (pos >= size())
            throw std::out_of_range("pos >= size()");
        return m_storage[pos];
    }

    // front() and back() require that this string is !empty(), otherwise -> UB
    char_type& front() { return m_storage[0]; }
    const char_type& front() const { return m_storage[0]; }
    char_type& back() { return m_storage[size() - 1]; }
    const char_type& back() const { return m_storage[size() - 1]; }


    /* ********************************** ITERATOR FUNCTIONS ********************************** */

    iterator begin() noexcept { return iterator(m_storage.data()); }
    const_iterator begin() const noexcept { return const_iterator(m_storage.data()); }
    const_iterator cbegin() const noexcept { return const_iterator(m_storage.data()); }
    iterator end() noexcept { return iterator(m_storage.data() + m_storage.length()); }
    const_iterator end() const noexcept
    {
        return const_iterator(m_storage.data() + m_storage.length());
    }
    const_iterator cend() const noexcept
    {
        return const_iterator(m_storage.data() + m_storage.length());
    }

    reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
    const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }
    const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator(end()); }
    reverse_iterator rend() noexcept { return reverse_iterator(begin()); }
    const_reverse_iterator rend() const noexcept { return const_reverse_iterator(begin()); }
    const_reverse_iterator crend() const noexcept { return const_reverse_iterator(begin()); }


    /* ********************************** CAPACITY FUNCTIONS ********************************** */

    bool empty() const noexcept { return size() == 0; }
    size_type max_size() const noexcept { return m_storage.max_size(); }
    size_type capacity() const noexcept { return max_size(); }
    size_type length() const noexcept { return m_storage.size(); }
    size_type size() const noexcept { return m_storage.size(); }

    void reserve(size_type new_cap = 0) { m_storage.reserve(new_cap); }
    void shrink_to_fit() { m_storage.shrink_to_fit(); }


    /* ********************************** OPERATIONS ********************************** */

    void clear() { m_storage.clear(); }
    // Not implemented here: insert, erase
    void push_back(char_type c) { m_storage.push_back(c); }
    void pop_back() { m_storage.pop_back(); }

    // sub-string
    this_type substr(size_type pos = 0, size_type count = npos) const
    {
        if (pos > size())
            throw std::out_of_range("pos > size()");
        return this_type(data() + pos, (count == npos ? size() - pos : count));
    }

    size_type copy(char_type* dest, size_type count, size_type pos = 0) const
    {
        if (pos > size())
            throw std::out_of_range("pos > size()");
        count = std::min(count, size() - pos);
        traits_type::copy(dest, data() + pos, count);
        return count;
    }

    void resize(size_type count) { resize(count, nul()); }
    void resize(size_type count, char_type ch) { m_storage.resize(count, ch); }
    void swap(this_type& other) noexcept { m_storage.swap(other.m_storage); }


    /* ********************************** APPENDING ********************************** */

    this_type& append(size_type count, char_type c)
    {
        m_storage.append(count, c);
        return *this;
    }
    this_type& append(const char_type* s, size_type count)
    {
        m_storage.append(s, count);
        return *this;
    }
    this_type& append(const char_type* s) { return append(s, traits_type::length(s)); }
    this_type& append(std::initializer_list<char_type> init)
    {
        return append(init.begin(), init.size());
    }

    template <typename InputIt, typename = checkInputIter<InputIt>>
    this_type& append(InputIt first, InputIt last)
    {
        m_storage.append(first, last);
        return *this;
    }

    template <typename StringClass, typename std::enable_if<is_compatible_string<
                                      char_type, size_type, StringClass>::value>::type* = nullptr>
    this_type& append(const StringClass& s)
    {
        return append(s.data(), s.size());
    }
    template <typename StringClass, typename std::enable_if<is_compatible_string<
                                      char_type, size_type, StringClass>::value>::type* = nullptr>
    this_type& append(const StringClass& s, size_type pos, size_type count = npos)
    {
        if (pos > s.size())
            throw std::out_of_range("pos > size()");
        return append(s.data() + pos, (count == npos ? s.size() - pos : count));
    }

    this_type& operator+=(char_type c)
    {
        push_back(c);
        return *this;
    }
    this_type& operator+=(const char_type* s) { return append(s); }
    this_type& operator+=(std::initializer_list<char_type> init) { return append(init); }
    template <typename StringClass, typename std::enable_if<is_compatible_string<
                                      char_type, size_type, StringClass>::value>::type* = nullptr>
    this_type& operator+=(const StringClass& s)
    {
        return append(s);
    }


    /* ********************************** COMPARISONS ********************************** */

    // base comparison
    static int _compare(const char_type* this_string, const size_type this_len,
                        const char_type* other_string, const size_type other_len)
    {
        const size_type cmplen = std::min(this_len, other_len);
        int r = traits_type::compare(this_string, other_string, cmplen);
        if (r == 0)
        {
            // compare the length difference if the strings are equal (so far)
            auto diff = static_cast<difference_type>(this_len - other_len);
            if (diff > std::numeric_limits<int>::max())
                diff = std::numeric_limits<int>::max();
            else if (diff < std::numeric_limits<int>::min())
                diff = std::numeric_limits<int>::min();
            r = static_cast<int>(diff);
        }
        return r;
    }

    int compare(const char_type* s) const
    {
        return _compare(data(), size(), s, traits_type::length(s));
    }

    int compare(size_type pos1, size_type count1, const char_type* s) const
    {
        return _compare(data() + pos1, count1, s, traits_type::length(s));
    }

    int compare(size_type pos1, size_type count1, const char_type* s, size_type count2) const
    {
        return _compare(data() + pos1, count1, s, count2);
    }

    // comparisons for (other) string-like classes
    template <typename StringClass, typename std::enable_if<is_compatible_string<
                                      char_type, size_type, StringClass>::value>::type* = nullptr>
    int compare(const StringClass& s) const
    {
        return _compare(data(), size(), s.data(), s.size());
    }

    template <typename StringClass, typename std::enable_if<is_compatible_string<
                                      char_type, size_type, StringClass>::value>::type* = nullptr>
    int compare(size_type pos1, size_type count1, const StringClass& s) const
    {
        return _compare(data() + pos1, count1, s.data(), s.size());
    }

    template <typename StringClass, typename std::enable_if<is_compatible_string<
                                      char_type, size_type, StringClass>::value>::type* = nullptr>
    int compare(size_type pos1, size_type count1, const StringClass& s, size_type pos2,
                size_type count2 = npos) const
    {
        return _compare(data() + pos1, count1, s.data() + pos2,
                        (count2 == npos ? s.size() : count2));
    }


    /* **************************** COMPARISON OPERATORS **************************** */

    // note: While we can implement these as non-members (which shall be preferred according to
    //       Effective C++), having member functions allows us to hand them down to sub-classes.

    // StringCore, const char_type*
    bool operator==(const char_type* rhs) const { return compare(rhs) == 0; }
    bool operator!=(const char_type* rhs) const { return compare(rhs) != 0; }
    bool operator<(const char_type* rhs) const { return compare(rhs) < 0; }
    bool operator<=(const char_type* rhs) const { return compare(rhs) <= 0; }
    bool operator>(const char_type* rhs) const { return compare(rhs) > 0; }
    bool operator>=(const char_type* rhs) const { return compare(rhs) >= 0; }

    // StringCore, StringCore
    bool operator==(const this_type& rhs) const { return compare(rhs) == 0; }
    bool operator!=(const this_type& rhs) const { return compare(rhs) != 0; }
    bool operator<(const this_type& rhs) const { return compare(rhs) < 0; }
    bool operator<=(const this_type& rhs) const { return compare(rhs) <= 0; }
    bool operator>(const this_type& rhs) const { return compare(rhs) > 0; }
    bool operator>=(const this_type& rhs) const { return compare(rhs) >= 0; }

    // StringCore, other string class
    template <typename StringClass, typename std::enable_if<is_compatible_string<
                                      char_type, size_type, StringClass>::value>::type* = nullptr>
    bool operator==(const StringClass& rhs) const
    {
        return compare(rhs) == 0;
    }
    template <typename StringClass, typename std::enable_if<is_compatible_string<
                                      char_type, size_type, StringClass>::value>::type* = nullptr>
    bool operator!=(const StringClass& rhs) const
    {
        return compare(rhs) != 0;
    }
    template <typename StringClass, typename std::enable_if<is_compatible_string<
                                      char_type, size_type, StringClass>::value>::type* = nullptr>
    bool operator<(const StringClass& rhs) const
    {
        return compare(rhs) < 0;
    }
    template <typename StringClass, typename std::enable_if<is_compatible_string<
                                      char_type, size_type, StringClass>::value>::type* = nullptr>
    bool operator<=(const StringClass& rhs) const
    {
        return compare(rhs) <= 0;
    }
    template <typename StringClass, typename std::enable_if<is_compatible_string<
                                      char_type, size_type, StringClass>::value>::type* = nullptr>
    bool operator>(const StringClass& rhs) const
    {
        return compare(rhs) > 0;
    }
    template <typename StringClass, typename std::enable_if<is_compatible_string<
                                      char_type, size_type, StringClass>::value>::type* = nullptr>
    bool operator>=(const StringClass& rhs) const
    {
        return compare(rhs) >= 0;
    }

    /* ********************************** SEARCH ********************************** */

    // note: the standard plays ping pong with "noexcept" - we'll mark them all for now...

    size_type find(const char_type* s, size_type pos, size_type count) const noexcept
    {
        const size_type mysize = size();
        const char_type* mydata = data();

        if (count == 0)
            return pos <= mysize ? pos : npos;

        if (count <= mysize)
        {
            for (; pos <= mysize - count; ++pos)
                if (traits_type::eq(mydata[pos], s[0]) &&
                    traits_type::compare(mydata + pos + 1, s + 1, count - 1) == 0)
                    return pos;
        }
        return npos;
    }

    size_type find(char_type ch, size_type pos = 0) const noexcept
    {
        size_type ret = npos;
        const size_type mysize = this->size();
        if (pos < mysize)
        {
            const char_type* mydata = data();
            const size_type n = mysize - pos;
            const char_type* p = traits_type::find(mydata + pos, n, ch);
            if (p)
                ret = static_cast<size_type>(p - mydata);
        }
        return ret;
    }

    size_type find(const char_type* s, size_type pos = 0) const noexcept
    {
        return find(s, pos, traits_type::length(s));
    }

    template <typename StringClass, typename std::enable_if<is_compatible_string<
                                      char_type, size_type, StringClass>::value>::type* = nullptr>
    size_type find(const StringClass& str, size_type pos = 0) const noexcept
    {
        return find(str.data(), pos, str.size());
    }


    size_type rfind(const char_type* s, size_type pos, size_type count) const noexcept
    {
        const size_type mysize = size();
        if (mysize && count <= mysize)
        {
            pos = std::min(size_type(mysize - count), pos);

            const char_type* d = data();
            for (const char_type* ptr = d + pos; ptr >= d; --ptr)
            {
                if (traits_type::compare(ptr, s, count) == 0)
                    return static_cast<size_type>(ptr - d);
            }
        }
        return npos;
    }

    size_type rfind(char_type ch, size_type pos = npos) const noexcept
    {
        if (size())
        {
            pos = std::min(pos, size() - 1);

            const char_type* d = data();
            for (const char_type* ptr = d + pos; ptr >= d; --ptr)
            {
                if (traits_type::eq(*ptr, ch))
                    return static_cast<size_type>(ptr - d);
            }
        }
        return npos;
    }

    size_type rfind(const char_type* s, size_type pos = npos) const noexcept
    {
        return rfind(s, pos, traits_type::length(s));
    }

    template <typename StringClass, typename std::enable_if<is_compatible_string<
                                      char_type, size_type, StringClass>::value>::type* = nullptr>
    size_type rfind(const StringClass& str, size_type pos = npos) const noexcept
    {
        return rfind(str.data(), pos, str.size());
    }


protected:
    /// the storage is responsible for storing the actual data, allocation etc.
    storage_type m_storage;
};


/* ********************************** COMPARISON OPERATORS ********************************** */
// These are the (non-member) functions, that can't be implemented as members.

// const char_type*, StringCore
template <typename StorageType>
bool operator==(const typename StorageType::char_type* lhs, const StringCore<StorageType>& rhs)
{
    return rhs.compare(lhs) == 0;
}
template <typename StorageType>
bool operator!=(const typename StorageType::char_type* lhs, const StringCore<StorageType>& rhs)
{
    return rhs.compare(lhs) != 0;
}
template <typename StorageType>
bool operator<(const typename StorageType::char_type* lhs, const StringCore<StorageType>& rhs)
{
    return rhs.compare(lhs) > 0;
}
template <typename StorageType>
bool operator<=(const typename StorageType::char_type* lhs, const StringCore<StorageType>& rhs)
{
    return rhs.compare(lhs) >= 0;
}
template <typename StorageType>
bool operator>(const typename StorageType::char_type* lhs, const StringCore<StorageType>& rhs)
{
    return rhs.compare(lhs) < 0;
}
template <typename StorageType>
bool operator>=(const typename StorageType::char_type* lhs, const StringCore<StorageType>& rhs)
{
    return rhs.compare(lhs) <= 0;
}


// other string class, StringCore
template <
  typename StringClass, typename StorageType,
  typename std::enable_if<is_compatible_string2<StorageType, StringClass>::value>::type* = nullptr>
bool operator==(const StringClass& lhs, const StringCore<StorageType>& rhs)
{
    return rhs.compare(lhs) == 0;
}
template <
  typename StringClass, typename StorageType,
  typename std::enable_if<is_compatible_string2<StorageType, StringClass>::value>::type* = nullptr>
bool operator!=(const StringClass& lhs, const StringCore<StorageType>& rhs)
{
    return rhs.compare(lhs) != 0;
}
template <
  typename StringClass, typename StorageType,
  typename std::enable_if<is_compatible_string2<StorageType, StringClass>::value>::type* = nullptr>
bool operator<(const StringClass& lhs, const StringCore<StorageType>& rhs)
{
    return rhs.compare(lhs) > 0;
}
template <
  typename StringClass, typename StorageType,
  typename std::enable_if<is_compatible_string2<StorageType, StringClass>::value>::type* = nullptr>
bool operator<=(const StringClass& lhs, const StringCore<StorageType>& rhs)
{
    return rhs.compare(lhs) >= 0;
}
template <
  typename StringClass, typename StorageType,
  typename std::enable_if<is_compatible_string2<StorageType, StringClass>::value>::type* = nullptr>
bool operator>(const StringClass& lhs, const StringCore<StorageType>& rhs)
{
    return rhs.compare(lhs) < 0;
}
template <
  typename StringClass, typename StorageType,
  typename std::enable_if<is_compatible_string2<StorageType, StringClass>::value>::type* = nullptr>
bool operator>=(const StringClass& lhs, const StringCore<StorageType>& rhs)
{
    return rhs.compare(lhs) <= 0;
}

// swap implementation
template <typename StorageType>
inline void swap(spsl::StringCore<StorageType>& lhs, spsl::StringCore<StorageType>& rhs)
{
    lhs.swap(rhs);
}

} // namespace spsl

namespace std
{

// partial specialization of std::hash
template <typename StorageType>
struct hash<spsl::StringCore<StorageType>>
{
    using argument_type = spsl::StringCore<StorageType>;

    hash() = default;
    hash(const hash&) = default;
    hash(hash&&) noexcept {}
    hash& operator=(const hash&) = default;
    hash& operator=(hash&&) noexcept { return *this; }
    ~hash() = default;

    size_t operator()(const argument_type& s) const
    {
        using char_type = typename spsl::StringCore<StorageType>::char_type;
        return spsl::hash::hash_impl(s.data(), s.size() * sizeof(char_type));
    }
};
} // namespace std

#endif /* SPSL_STRINGCORE_HPP_ */
