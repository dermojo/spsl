/**
 * @file    Special Purpose Strings Library: stringbase.hpp
 * @author  Daniel Evers
 * @brief   Core string class functionality
 * @license MIT
 */

#ifndef SPSL_STRINGBASE_HPP_
#define SPSL_STRINGBASE_HPP_

#include <algorithm>
#include <initializer_list>
#include <iterator>
#include <limits>
#include <stdexcept>
#include <string_view>

#include "spsl/type_traits.hpp"


namespace spsl
{

/**
 * This class extends the core string implementation to provide all methods specified for
 * @c std::basic_string, especially the insert, erase and replace methods.
 *
 * The methods are mostly implemented in the underlying StorageType to address for implementation
 * specific needs.
 */
template <typename StorageType>
class StringBase
{
private:
    /// the storage is responsible for storing the actual data, allocation etc.
    StorageType m_storage;

public:
    /// alias for the typing impaired :)
    using this_type = StringBase<StorageType>;

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

    // view type alias, using the same character type
    using string_view_type = std::basic_string_view<char_type>;


    // For now, we use plain pointers. If this isn't sufficient, we need to roll our own...
    using iterator = pointer;
    using const_iterator = const_pointer;
    using const_reverse_iterator = typename std::reverse_iterator<const_iterator>;
    using reverse_iterator = typename std::reverse_iterator<iterator>;


    static constexpr size_type npos = static_cast<size_type>(-1);
    static constexpr char_type nul = static_cast<char_type>(0);


    /* ************************************ CONSTRUCTORS ************************************ */

    StringBase() : m_storage() {}
    StringBase(const char_type* s) : m_storage() { assign(s); }
    StringBase(const char_type* s, size_type n) : m_storage() { assign(s, n); }
    StringBase(size_type numRepeat, char_type ch) : m_storage()
    {
        m_storage.insert(0, numRepeat, ch);
    }
    StringBase(std::initializer_list<char_type> init) : m_storage()
    {
        assign(init.begin(), init.size());
    }
    template <class InputIt, typename = checkInputIter<InputIt>>
    StringBase(InputIt first, InputIt last) : m_storage()
    {
        assign(first, last);
    }


    // default destructor, move and copy
    ~StringBase() = default;
    StringBase(const this_type&) = default;
    StringBase(this_type&& rhs) noexcept : StringBase() { this->swap(rhs); }

    /// construct from another storage instance
    explicit StringBase(const storage_type& storage) : m_storage(storage) {}

    /// construct from another string type
    explicit StringBase(const string_view_type& s) : StringBase(s.data(), s.size()) {}

    /* ********************************** ASSIGNMENT OPERATORS ********************************** */

    this_type& operator=(const char_type* s)
    {
        assign(s);
        return *this;
    }
    this_type& operator=(char_type c)
    {
        assign(&c, 1);
        return *this;
    }
    this_type& operator=(std::initializer_list<char_type> init)
    {
        assign(init.begin(), init.size());
        return *this;
    }

    // default copy & move
    this_type& operator=(const this_type&) = default;
    this_type& operator=(this_type&& rhs) noexcept
    {
        this->swap(rhs);
        return *this;
    }

    /// allow assignment from another string type
    this_type& operator=(const string_view_type& s)
    {
        assign(s.data(), s.size());
        return *this;
    }


    /* ********************************** CAPACITY FUNCTIONS ********************************** */

    [[nodiscard]] bool empty() const noexcept { return size() == 0; }
    [[nodiscard]] size_type max_size() const noexcept { return m_storage.max_size(); }
    [[nodiscard]] size_type capacity() const noexcept { return max_size(); }
    [[nodiscard]] size_type length() const noexcept { return m_storage.size(); }
    [[nodiscard]] size_type size() const noexcept { return m_storage.size(); }

    void reserve(size_type new_cap = 0) { m_storage.reserve(new_cap); }
    void shrink_to_fit() { m_storage.shrink_to_fit(); }


    /* ********************************** ACCESS FUNCTIONS ********************************** */

    /// @return a pointer to the internal C-string (NUL terminated)
    [[nodiscard]] const char_type* c_str() const { return m_storage.data(); }
    [[nodiscard]] const char_type* data() const { return m_storage.data(); }
    [[nodiscard]] char_type* data() { return m_storage.data(); }

    [[nodiscard]] char_type& operator[](size_type pos) { return m_storage[pos]; }
    [[nodiscard]] const char_type& operator[](size_type pos) const { return m_storage[pos]; }
    [[nodiscard]] char_type& at(size_type pos)
    {
        if (pos >= size())
            throw std::out_of_range("pos >= size()");
        return m_storage[pos];
    }
    [[nodiscard]] const char_type& at(size_type pos) const
    {
        if (pos >= size())
            throw std::out_of_range("pos >= size()");
        return m_storage[pos];
    }

    // front() and back() require that this string is !empty(), otherwise -> UB
    [[nodiscard]] char_type& front() { return m_storage[0]; }
    [[nodiscard]] const char_type& front() const { return m_storage[0]; }
    [[nodiscard]] char_type& back() { return m_storage[size() - 1]; }
    [[nodiscard]] const char_type& back() const { return m_storage[size() - 1]; }

    // allow conversion to a view
    constexpr operator string_view_type() const { return string_view_type(data(), size()); }

    /* ********************************** ITERATOR FUNCTIONS ********************************** */

    [[nodiscard]] iterator begin() noexcept { return iterator(m_storage.data()); }
    [[nodiscard]] const_iterator begin() const noexcept { return const_iterator(m_storage.data()); }
    [[nodiscard]] const_iterator cbegin() const noexcept
    {
        return const_iterator(m_storage.data());
    }
    [[nodiscard]] iterator end() noexcept { return iterator(m_storage.data() + m_storage.size()); }
    [[nodiscard]] const_iterator end() const noexcept
    {
        return const_iterator(m_storage.data() + m_storage.size());
    }
    [[nodiscard]] const_iterator cend() const noexcept
    {
        return const_iterator(m_storage.data() + m_storage.size());
    }

    [[nodiscard]] reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
    [[nodiscard]] const_reverse_iterator rbegin() const noexcept
    {
        return const_reverse_iterator(end());
    }
    [[nodiscard]] const_reverse_iterator crbegin() const noexcept
    {
        return const_reverse_iterator(end());
    }
    [[nodiscard]] reverse_iterator rend() noexcept { return reverse_iterator(begin()); }
    [[nodiscard]] const_reverse_iterator rend() const noexcept
    {
        return const_reverse_iterator(begin());
    }
    [[nodiscard]] const_reverse_iterator crend() const noexcept
    {
        return const_reverse_iterator(begin());
    }


    /* ********************************** OPERATIONS ********************************** */

    void clear() { m_storage.clear(); }
    // Not implemented here: insert, erase
    void push_back(char_type c) { m_storage.push_back(c); }
    void pop_back() { m_storage.pop_back(); }

    // sub-string
    [[nodiscard]] this_type substr(size_type pos = 0, size_type count = npos) const
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

    void resize(size_type count) { resize(count, nul); }
    void resize(size_type count, char_type ch) { m_storage.resize(count, ch); }
    void swap(this_type& other) noexcept { m_storage.swap(other.m_storage); }


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

    [[nodiscard]] int compare(const char_type* s) const
    {
        return _compare(data(), size(), s, traits_type::length(s));
    }

    [[nodiscard]] int compare(size_type pos1, size_type count1, const char_type* s) const
    {
        return _compare(data() + pos1, count1, s, traits_type::length(s));
    }

    [[nodiscard]] int compare(size_type pos1, size_type count1, const char_type* s,
                              size_type count2) const
    {
        return _compare(data() + pos1, count1, s, count2);
    }

    // comparisons for (other) string types
    [[nodiscard]] int compare(const string_view_type& s) const
    {
        return _compare(data(), size(), s.data(), s.size());
    }

    [[nodiscard]] int compare(size_type pos1, size_type count1, const string_view_type& s) const
    {
        return _compare(data() + pos1, count1, s.data(), s.size());
    }

    [[nodiscard]] int compare(size_type pos1, size_type count1, const string_view_type& s,
                              size_type pos2, size_type count2 = npos) const
    {
        return _compare(data() + pos1, count1, s.data() + pos2,
                        (count2 == npos ? s.size() : count2));
    }


    /* **************************** COMPARISON OPERATORS **************************** */

    // note: While we can implement these as non-members (which shall be preferred according to
    //       Effective C++), having member functions allows us to hand them down to sub-classes.

    // TODO: update to hidden friends

    // StringBase, const char_type*
    bool operator==(const char_type* rhs) const { return compare(rhs) == 0; }
    bool operator!=(const char_type* rhs) const { return compare(rhs) != 0; }
    bool operator<(const char_type* rhs) const { return compare(rhs) < 0; }
    bool operator<=(const char_type* rhs) const { return compare(rhs) <= 0; }
    bool operator>(const char_type* rhs) const { return compare(rhs) > 0; }
    bool operator>=(const char_type* rhs) const { return compare(rhs) >= 0; }

    // StringBase, StringBase
    bool operator==(const this_type& rhs) const { return compare(rhs) == 0; }
    bool operator!=(const this_type& rhs) const { return compare(rhs) != 0; }
    bool operator<(const this_type& rhs) const { return compare(rhs) < 0; }
    bool operator<=(const this_type& rhs) const { return compare(rhs) <= 0; }
    bool operator>(const this_type& rhs) const { return compare(rhs) > 0; }
    bool operator>=(const this_type& rhs) const { return compare(rhs) >= 0; }

    // StringBase, other string type
    bool operator==(const string_view_type& rhs) const { return compare(rhs) == 0; }
    bool operator!=(const string_view_type& rhs) const { return compare(rhs) != 0; }
    bool operator<(const string_view_type& rhs) const { return compare(rhs) < 0; }
    bool operator<=(const string_view_type& rhs) const { return compare(rhs) <= 0; }
    bool operator>(const string_view_type& rhs) const { return compare(rhs) > 0; }
    bool operator>=(const string_view_type& rhs) const { return compare(rhs) >= 0; }


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

    /// allow assignment from another string type
    this_type& assign(const string_view_type& s) { return assign(s.data(), s.size()); }

    /// same, but with pos and count
    this_type& assign(const string_view_type& s, size_type pos, size_type count = npos)
    {
        if (pos > s.size())
            throw std::out_of_range("pos > size()");
        return assign(s.data() + pos, (count == npos ? s.size() - pos : count));
    }


    /* ********************************** INSERT FUNCTIONS ********************************** */

    // repeated character
    this_type& insert(size_type index, size_type count, char_type ch)
    {
        m_storage.insert(index, count, ch);
        return *this;
    }
    // C-string
    this_type& insert(size_type index, const char_type* s)
    {
        return insert(index, s, traits_type::length(s));
    }
    // C-string + length
    this_type& insert(size_type index, const char_type* s, size_type count)
    {
        m_storage.insert(index, s, s + count);
        return *this;
    }
    // another string type
    this_type& insert(size_type index, const string_view_type& s)
    {
        return insert(index, s.data(), s.size());
    }
    // another string-like class with index and count
    this_type& insert(size_type index, const string_view_type& s, size_type index_str,
                      size_type count = npos)
    {
        return insert(index, s.data() + index_str, std::min(s.size() - index_str, count));
    }

    // iterator variants
    iterator insert(const_iterator pos, char_type ch) { return insert(pos, 1, ch); }
    iterator insert(const_iterator pos, size_type count, char_type ch)
    {
        const auto offset = static_cast<size_t>(pos - begin());
        m_storage.insert(offset, count, ch);
        return begin() + offset;
    }
    template <class InputIt, typename = checkInputIter<InputIt>>
    iterator insert(const_iterator pos, InputIt first, InputIt last)
    {
        const auto offset = static_cast<size_t>(pos - begin());
        m_storage.insert(offset, first, last);
        return begin() + offset;
    }
    iterator insert(const_iterator pos, std::initializer_list<char_type> ilist)
    {
        return insert(pos, ilist.begin(), ilist.end());
    }


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

    this_type& append(const string_view_type& s) { return append(s.data(), s.size()); }
    this_type& append(const string_view_type& s, size_type pos, size_type count = npos)
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
    this_type& operator+=(const string_view_type& s) { return append(s); }


    /* ********************************** ERASE FUNCTIONS ********************************** */

    this_type& erase(size_type index = 0, size_type count = npos)
    {
        if (index > size())
            throw std::out_of_range("index > size()");

        m_storage.erase(index, std::min(count, size() - index));
        return *this;
    }
    iterator erase(const_iterator position)
    {
        if (position < cbegin() || position > cend())
            throw std::out_of_range("invalid iterator in erase()");

        auto pos = static_cast<size_type>(position - cbegin());
        m_storage.erase(pos, 1);
        return begin() + pos;
    }
    iterator erase(const_iterator first, const_iterator last)
    {
        if (first < cbegin() || last > cend() || first > last)
            throw std::out_of_range("invalid iterator(s) in erase()");

        auto pos = static_cast<size_type>(first - cbegin());
        auto cnt = static_cast<size_type>(last - first);
        m_storage.erase(pos, cnt);
        return begin() + pos;
    }


    /* ********************************** REPLACE FUNCTIONS ********************************** */

    // the first 3 methods wrap the actual implementation in the storage class,
    // the others are just wrappers for these 3

    this_type& replace(size_type pos, size_type count, const char_type* cstr, size_type count2)
    {
        if (pos > size())
            throw std::out_of_range("replace: pos > size()");
        if (pos + count > size())
            throw std::out_of_range("replace: pos + count > size()");

        // forward to the storage implementation...
        m_storage.replace(pos, count, cstr, count2);
        return *this;
    }
    this_type& replace(size_type pos, size_type count, size_type count2, char_type ch)
    {
        if (pos > size())
            throw std::out_of_range("replace: pos > size()");
        if (pos + count > size())
            throw std::out_of_range("replace: pos + count > size()");

        // forward to the storage implementation...
        m_storage.replace(pos, count, count2, ch);
        return *this;
    }
    template <class InputIt, typename = checkInputIter<InputIt>>
    this_type& replace(const_iterator first, const_iterator last, InputIt first2, InputIt last2)
    {
        auto pos = static_cast<size_type>(first - data());
        auto count = static_cast<size_type>(last - first);
        if (pos > size())
            throw std::out_of_range("replace: pos > size()");
        if (pos + count > size())
            throw std::out_of_range("replace: pos + count > size()");

        // forward to the storage implementation...
        m_storage.replace(pos, count, first2, last2);
        return *this;
    }

    this_type& replace(const_iterator first, const_iterator last, const char_type* cstr,
                       size_type count2)
    {
        return replace(static_cast<size_type>(first - data()), static_cast<size_type>(last - first),
                       cstr, count2);
    }
    this_type& replace(size_type pos, size_type count, const char_type* cstr)
    {
        return replace(pos, count, cstr, traits_type::length(cstr));
    }
    this_type& replace(const_iterator first, const_iterator last, const char_type* cstr)
    {
        return replace(static_cast<size_type>(first - data()), static_cast<size_type>(last - first),
                       cstr, traits_type::length(cstr));
    }
    this_type& replace(const_iterator first, const_iterator last, size_type count2, char_type ch)
    {
        return replace(static_cast<size_type>(first - data()), static_cast<size_type>(last - first),
                       count2, ch);
    }
    this_type& replace(const_iterator first, const_iterator last,
                       std::initializer_list<char_type> ilist)
    {
        return replace(static_cast<size_type>(first - data()), static_cast<size_type>(last - first),
                       ilist.begin(), ilist.size());
    }

    this_type& replace(size_type pos, size_type count, const string_view_type& s)
    {
        return replace(pos, count, s.data(), s.size());
    }

    this_type& replace(const_iterator first, const_iterator last, const string_view_type& s)
    {
        return replace(static_cast<size_type>(first - data()), static_cast<size_type>(last - first),
                       s.data(), s.size());
    }

    this_type& replace(size_type pos, size_type count, const string_view_type& s, size_type pos2,
                       size_type count2 = npos)
    {
        return replace(pos, count, s.data() + pos2, std::min(count2, s.size() - pos2));
    }


    /* ********************************** FIND FUNCTIONS ********************************** */

    // Note: The standard plays ping-pong with "noexcept". We'll simply declare them all "noexcept"
    //       and don't raise any exceptions. I.e. there's no bounds checking...

    [[nodiscard]] size_type find(const char_type* s, size_type pos, size_type count) const noexcept
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

    [[nodiscard]] size_type find(char_type ch, size_type pos = 0) const noexcept
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

    [[nodiscard]] size_type find(const char_type* s, size_type pos = 0) const noexcept
    {
        return find(s, pos, traits_type::length(s));
    }

    [[nodiscard]] size_type find(const string_view_type& str, size_type pos = 0) const noexcept
    {
        return find(str.data(), pos, str.size());
    }


    [[nodiscard]] size_type rfind(const char_type* s, size_type pos, size_type count) const noexcept
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

    [[nodiscard]] size_type rfind(char_type ch, size_type pos = npos) const noexcept
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

    [[nodiscard]] size_type rfind(const char_type* s, size_type pos = npos) const noexcept
    {
        return rfind(s, pos, traits_type::length(s));
    }

    [[nodiscard]] size_type rfind(const string_view_type& str, size_type pos = npos) const noexcept
    {
        return rfind(str.data(), pos, str.size());
    }


    // FIND_FIRST_OF

    [[nodiscard]] size_type find_first_of(const char_type* s, size_type pos,
                                          size_type count) const noexcept
    {
        for (size_type index = pos; index < size(); ++index)
        {
            for (size_type ind2 = 0; ind2 < count; ++ind2)
            {
                if (traits_type::eq(m_storage[index], s[ind2]))
                    return index;
            }
        }
        return npos;
    }

    [[nodiscard]] size_type find_first_of(char_type ch, size_type pos = 0) const noexcept
    {
        return find(ch, pos);
    }

    [[nodiscard]] size_type find_first_of(const char_type* s, size_type pos = 0) const noexcept
    {
        return find_first_of(s, pos, traits_type::length(s));
    }
    [[nodiscard]] size_type find_first_of(const string_view_type& s,
                                          size_type pos = 0) const noexcept
    {
        return find_first_of(s.data(), pos, s.size());
    }

    // FIND_FIRST_NOT_OF

    [[nodiscard]] size_type find_first_not_of(const char_type* s, size_type pos,
                                              size_type count) const noexcept
    {
        for (size_type index = pos; index < size(); ++index)
        {
            bool found = false;
            for (size_type ind2 = 0; ind2 < count && !found; ++ind2)
            {
                if (traits_type::eq(m_storage[index], s[ind2]))
                    found = true;
            }
            if (!found)
                return index;
        }
        return npos;
    }
    [[nodiscard]] size_type find_first_not_of(char_type ch, size_type pos = 0) const noexcept
    {
        for (size_type index = pos; index < size(); ++index)
        {
            if (!traits_type::eq(m_storage[index], ch))
                return index;
        }
        return npos;
    }

    [[nodiscard]] size_type find_first_not_of(const char_type* s, size_type pos = 0) const noexcept
    {
        return find_first_not_of(s, pos, traits_type::length(s));
    }
    [[nodiscard]] size_type find_first_not_of(const string_view_type& s,
                                              size_type pos = 0) const noexcept
    {
        return find_first_not_of(s.data(), pos, s.size());
    }

    // FIND_LAST_OF

    [[nodiscard]] size_type find_last_of(const char_type* s, size_type pos,
                                         size_type count) const noexcept
    {
        if (size())
        {
            // handle npos
            pos = std::min(pos, size() - 1);

            const char_type* d = data();
            for (const char_type* ptr = d + pos; ptr >= d; --ptr)
            {
                for (size_type ind2 = 0; ind2 < count; ++ind2)
                {
                    if (traits_type::eq(*ptr, s[ind2]))
                        return static_cast<size_type>(ptr - d);
                }
            }
        }
        return npos;
    }
    [[nodiscard]] size_type find_last_of(char_type ch, size_type pos = npos) const noexcept
    {
        return rfind(ch, pos);
    }

    [[nodiscard]] size_type find_last_of(const char_type* s, size_type pos = npos) const noexcept
    {
        return find_last_of(s, pos, traits_type::length(s));
    }
    [[nodiscard]] size_type find_last_of(const string_view_type& s,
                                         size_type pos = npos) const noexcept
    {
        return find_last_of(s.data(), pos, s.size());
    }

    // FIND_LAST_NOT_OF

    [[nodiscard]] size_type find_last_not_of(const char_type* s, size_type pos,
                                             size_type count) const noexcept
    {
        if (size())
        {
            // handle npos
            pos = std::min(pos, size() - 1);

            const char_type* d = data();
            for (const char_type* ptr = d + pos; ptr >= d; --ptr)
            {
                bool found = false;
                for (size_type ind2 = 0; ind2 < count && !found; ++ind2)
                {
                    if (traits_type::eq(*ptr, s[ind2]))
                        found = true;
                }
                if (!found)
                    return static_cast<size_type>(ptr - d);
            }
        }
        return npos;
    }
    [[nodiscard]] size_type find_last_not_of(char_type ch, size_type pos = npos) const noexcept
    {
        if (size())
        {
            // handle npos
            pos = std::min(pos, size() - 1);

            const char_type* d = data();
            for (const char_type* ptr = d + pos; ptr >= d; --ptr)
            {
                if (!traits_type::eq(*ptr, ch))
                    return static_cast<size_type>(ptr - d);
            }
        }
        return npos;
    }

    [[nodiscard]] size_type find_last_not_of(const char_type* s,
                                             size_type pos = npos) const noexcept
    {
        return find_last_not_of(s, pos, traits_type::length(s));
    }
    [[nodiscard]] size_type find_last_not_of(const string_view_type& s,
                                             size_type pos = npos) const noexcept
    {
        return find_last_not_of(s.data(), pos, s.size());
    }
};


/* ********************************** COMPARISON OPERATORS ********************************** */
// These are the (non-member) functions, that can't be implemented as members.

// SOMETHING, StringBase
template <typename OtherString, typename StorageType>
bool operator==(const OtherString& lhs, const StringBase<StorageType>& rhs)
{
    return rhs.compare(lhs) == 0;
}
template <typename OtherString, typename StorageType>
bool operator!=(const OtherString& lhs, const StringBase<StorageType>& rhs)
{
    return rhs.compare(lhs) != 0;
}
template <typename OtherString, typename StorageType>
bool operator<(const OtherString& lhs, const StringBase<StorageType>& rhs)
{
    return rhs.compare(lhs) > 0;
}
template <typename OtherString, typename StorageType>
bool operator<=(const OtherString& lhs, const StringBase<StorageType>& rhs)
{
    return rhs.compare(lhs) >= 0;
}
template <typename OtherString, typename StorageType>
bool operator>(const OtherString& lhs, const StringBase<StorageType>& rhs)
{
    return rhs.compare(lhs) < 0;
}
template <typename OtherString, typename StorageType>
bool operator>=(const OtherString& lhs, const StringBase<StorageType>& rhs)
{
    return rhs.compare(lhs) <= 0;
}


// swap implementation
template <typename StorageType>
inline void swap(spsl::StringBase<StorageType>& lhs, spsl::StringBase<StorageType>& rhs)
{
    lhs.swap(rhs);
}

} // namespace spsl

namespace std
{

// partial specialization of std::hash
template <typename StorageType>
struct hash<spsl::StringBase<StorageType>>
{
    using argument_type = spsl::StringBase<StorageType>;

    size_t operator()(const argument_type& s) const noexcept
    {
        // re-use string_view's hash
        using char_type = typename argument_type::char_type;
        using view_type = basic_string_view<char_type>;
        return hash<view_type>()(view_type(s.data(), s.size()));
    }
};

} // namespace std

#endif /* SPSL_STRINGBASE_HPP_ */
