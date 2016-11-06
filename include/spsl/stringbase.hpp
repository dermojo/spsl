/**
 * @file    Special Purpose Strings Library: stringbase.hpp
 * @author  Daniel Evers
 * @brief   Core string class functionality
 * @license MIT
 */

#ifndef SPSL_STRINGBASE_HPP_
#define SPSL_STRINGBASE_HPP_

#include "spsl/stringcore.hpp"

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
class StringBase : public StringCore<StorageType>
{
public:
    /// alias for the typing impaired :)
    using this_type = StringBase<StorageType>;

    // inherit all typedefs from the base class
    using base_type = StringCore<StorageType>;
    using storage_type = StorageType;
    using char_type = typename base_type::char_type;
    using size_type = typename base_type::size_type;
    using difference_type = typename base_type::difference_type;
    using traits_type = typename base_type::traits_type;
    using value_type = typename base_type::value_type;
    using reference = typename base_type::reference;
    using const_reference = typename base_type::const_reference;
    using pointer = typename base_type::pointer;
    using const_pointer = typename base_type::const_pointer;
    using allocator_type = typename base_type::allocator_type;

    using iterator = typename base_type::iterator;
    using const_iterator = typename base_type::const_iterator;
    using const_reverse_iterator = typename base_type::const_reverse_iterator;
    using reverse_iterator = typename base_type::reverse_iterator;

    // "inherit" everything else as well
    using base_type::npos;
    using base_type::nul;
    using base_type::assign;
    using base_type::c_str;
    using base_type::data;
    using base_type::operator[];
    using base_type::at;
    using base_type::front;
    using base_type::back;
    using base_type::begin;
    using base_type::cbegin;
    using base_type::rbegin;
    using base_type::crbegin;
    using base_type::end;
    using base_type::cend;
    using base_type::rend;
    using base_type::crend;
    using base_type::empty;
    using base_type::max_size;
    using base_type::capacity;
    using base_type::length;
    using base_type::size;
    using base_type::reserve;
    using base_type::shrink_to_fit;
    using base_type::clear;
    using base_type::push_back;
    using base_type::pop_back;
    using base_type::substr;
    using base_type::copy;
    using base_type::resize;
    using base_type::swap;
    using base_type::append;
    using base_type::operator+=;
    using base_type::compare;
    using base_type::find;
    using base_type::rfind;

    /* ************************************ CONSTRUCTORS ************************************ */

    StringBase() : base_type() {}
    StringBase(const char_type* s) : base_type(s) {}
    StringBase(const char_type* s, size_type n) : base_type(s, n) {}
    StringBase(size_type numRepeat, char_type ch) : base_type(numRepeat, ch) {}
    StringBase(std::initializer_list<char_type> init) : base_type(init) {}

    // default destructor, move and copy
    ~StringBase() = default;
    StringBase(const this_type&) = default;
#ifdef SPSL_HAS_DEFAULT_MOVE
    StringBase(this_type&&) = default;
#else
    StringBase(this_type&& rhs) { this->swap(rhs); }
#endif

    /// construct from another storage instance
    explicit StringBase(const storage_type& storage) : base_type(storage) {}

    /// construct from another string-like container (may even be a vector...)
    template <typename StringClass,
              typename std::enable_if<is_compatible_string<char_type, size_type, StringClass>::value>::type* = nullptr>
    explicit StringBase(const StringClass& s) : base_type(s)
    {
    }


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
#ifdef SPSL_HAS_DEFAULT_MOVE
    this_type& operator=(this_type&&) = default;
#else
    this_type& operator=(this_type&& rhs)
    {
        this->swap(rhs);
        return *this;
    }
#endif

    /// allow assignment from another string-like container (may even be a vector...)
    template <typename StringClass,
              typename std::enable_if<is_compatible_string<char_type, size_type, StringClass>::value>::type* = nullptr>
    this_type& operator=(const StringClass& s)
    {
        assign(s.data(), s.size());
        return *this;
    }


    /* **************************** COMPARISON OPERATORS **************************** */

    // note: While we can implement these as non-members (which shall be preferred according to
    //       Effective C++), having member functions allows us to hand them down to sub-classes.

    using base_type::operator==;
    using base_type::operator!=;
    using base_type::operator<;
    using base_type::operator<=;
    using base_type::operator>;
    using base_type::operator>=;

    // StringBase, const char_type* --> inherited from StringCore

    // StringBase, StringBase
    bool operator==(const this_type& rhs) const { return compare(rhs) == 0; }
    bool operator!=(const this_type& rhs) const { return compare(rhs) != 0; }
    bool operator<(const this_type& rhs) const { return compare(rhs) < 0; }
    bool operator<=(const this_type& rhs) const { return compare(rhs) <= 0; }
    bool operator>(const this_type& rhs) const { return compare(rhs) > 0; }
    bool operator>=(const this_type& rhs) const { return compare(rhs) >= 0; }

    // StringBase, other string class --> inherited from StringCore


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
    // another string-like class
    template <typename StringClass,
              typename std::enable_if<is_compatible_string<char_type, size_type, StringClass>::value>::type* = nullptr>
    this_type& insert(size_type index, const StringClass& s)
    {
        return insert(index, s.data(), s.size());
    }
    // another string-like class with index and count
    template <typename StringClass,
              typename std::enable_if<is_compatible_string<char_type, size_type, StringClass>::value>::type* = nullptr>
    this_type& insert(size_type index, const StringClass& s, size_type index_str,
                      size_type count = npos)
    {
        return insert(index, s.data() + index_str, std::min(s.size() - index_str, count));
    }

    // iterator variants
    iterator insert(const_iterator pos, char_type ch) { return insert(pos, 1, ch); }
    iterator insert(const_iterator pos, size_type count, char_type ch)
    {
        m_storage.insert(pos - begin(), count, ch);
        return pos;
    }
    template <class InputIt>
    iterator insert(const_iterator pos, InputIt first, InputIt last)
    {
        m_storage.insert(pos - begin(), first, last);
        return begin() + index;
    }
    iterator insert(const_iterator pos, std::initializer_list<char_type> ilist)
    {
        return insert(pos, ilist.begin(), ilist.end());
    }


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

        m_storage.erase(position - cbegin(), 1);
        return position;
    }
    iterator erase(const_iterator first, const_iterator last)
    {
        if (first < cbegin() || last > cend() || first > last)
            throw std::out_of_range("invalid iterator(s) in erase()");

        m_storage.erase(first - cbegin(), last - first);
        return first;
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
    template <class InputIt>
    this_type& replace(const_iterator first, const_iterator last, InputIt first2, InputIt last2)
    {
        size_type pos = first - data();
        size_type count = last - first;
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
        return replace(first - data(), last - first, cstr, count2);
    }
    this_type& replace(size_type pos, size_type count, const char_type* cstr)
    {
        return replace(pos, count, cstr, traits_type::length(cstr));
    }
    this_type& replace(const_iterator first, const_iterator last, const char_type* cstr)
    {
        return replace(first - data(), last - first, cstr, traits_type::length(cstr));
    }
    this_type& replace(const_iterator first, const_iterator last, size_type count2, char_type ch)
    {
        return replace(first - data(), last - first, count2, ch);
    }
    this_type& replace(const_iterator first, const_iterator last,
                       std::initializer_list<char_type> ilist)
    {
        return replace(first - data(), last - first, ilist.begin(), ilist.size());
    }

    template <typename StringClass,
              typename std::enable_if<is_compatible_string<char_type, size_type, StringClass>::value>::type* = nullptr>
    this_type& replace(size_type pos, size_type count, const StringClass& s)
    {
        return replace(pos, count, s.data(), s.size());
    }

    template <typename StringClass,
              typename std::enable_if<is_compatible_string<char_type, size_type, StringClass>::value>::type* = nullptr>
    this_type& replace(const_iterator first, const_iterator last, const StringClass& s)
    {
        return replace(first - data(), last - first, s.data(), s.size());
    }

    template <typename StringClass,
              typename std::enable_if<is_compatible_string<char_type, size_type, StringClass>::value>::type* = nullptr>
    this_type& replace(size_type pos, size_type count, const StringClass& s, size_type pos2,
                       size_type count2 = npos)
    {
        return replace(pos, count, s.data() + pos2, std::min(count2, s.size() - pos2));
    }


    /* ********************************** FIND FUNCTIONS ********************************** */

    // Note: The standard plays ping-pong with "noexcept". We'll simply declare them all "noexcept"
    //       and don't raise any exceptions. I.e. there's no bounds checking...

    // FIND_FIRST_OF

    size_type find_first_of(const char_type* s, size_type pos, size_type count) const noexcept
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

    size_type find_first_of(char_type ch, size_type pos = 0) const noexcept
    {
        return find(ch, pos);
    }

    size_type find_first_of(const char_type* s, size_type pos = 0) const noexcept
    {
        return find_first_of(s, pos, traits_type::length(s));
    }
    template <typename StringClass,
              typename std::enable_if<is_compatible_string<char_type, size_type, StringClass>::value>::type* = nullptr>
    size_type find_first_of(const StringClass& s, size_type pos = 0) const noexcept
    {
        return find_first_of(s.data(), pos, s.size());
    }

    // FIND_FIRST_NOT_OF

    size_type find_first_not_of(const char_type* s, size_type pos, size_type count) const noexcept
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
    size_type find_first_not_of(char_type ch, size_type pos = 0) const noexcept
    {
        for (size_type index = pos; index < size(); ++index)
        {
            if (!traits_type::eq(m_storage[index], ch))
                return index;
        }
        return npos;
    }

    size_type find_first_not_of(const char_type* s, size_type pos = 0) const noexcept
    {
        return find_first_not_of(s, pos, traits_type::length(s));
    }
    template <typename StringClass,
              typename std::enable_if<is_compatible_string<char_type, size_type, StringClass>::value>::type* = nullptr>
    size_type find_first_not_of(const StringClass& s, size_type pos = 0) const noexcept
    {
        return find_first_not_of(s.data(), pos, s.size());
    }

    // FIND_LAST_OF

    size_type find_last_of(const char_type* s, size_type pos, size_type count) const noexcept
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
                        return (ptr - d);
                }
            }
        }
        return npos;
    }
    size_type find_last_of(char_type ch, size_type pos = npos) const noexcept
    {
        return rfind(ch, pos);
    }

    size_type find_last_of(const char_type* s, size_type pos = npos) const noexcept
    {
        return find_last_of(s, pos, traits_type::length(s));
    }
    template <typename StringClass,
              typename std::enable_if<is_compatible_string<char_type, size_type, StringClass>::value>::type* = nullptr>
    size_type find_last_of(const StringClass& s, size_type pos = npos) const noexcept
    {
        return find_last_of(s.data(), pos, s.size());
    }

    // FIND_LAST_NOT_OF

    size_type find_last_not_of(const char_type* s, size_type pos, size_type count) const noexcept
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
                    return (ptr - d);
            }
        }
        return npos;
    }
    size_type find_last_not_of(char_type ch, size_type pos = npos) const noexcept
    {
        if (size())
        {
            // handle npos
            pos = std::min(pos, size() - 1);

            const char_type* d = data();
            for (const char_type* ptr = d + pos; ptr >= d; --ptr)
            {
                if (!traits_type::eq(*ptr, ch))
                    return (ptr - d);
            }
        }
        return npos;
    }

    size_type find_last_not_of(const char_type* s, size_type pos = npos) const noexcept
    {
        return find_last_not_of(s, pos, traits_type::length(s));
    }
    template <typename StringClass, typename std::enable_if<is_compatible_string<
                                      char_type, size_type, StringClass>::value>::type* = nullptr>
    size_type find_last_not_of(const StringClass& s, size_type pos = npos) const noexcept
    {
        return find_last_not_of(s.data(), pos, s.size());
    }

protected:
    using base_type::m_storage;
};

}

namespace std
{

// partial specialization of std::swap
template <typename StorageType>
inline void swap(spsl::StringBase<StorageType>& lhs, spsl::StringBase<StorageType>& rhs)
{
    lhs.swap(rhs);
}

// partial specialization of std::hash
template <typename StorageType>
struct hash<spsl::StringBase<StorageType>>
{
    using argument_type = spsl::StringBase<StorageType>;

    hash() = default;
    hash(const hash&) = default;
#ifdef SPSL_HAS_DEFAULT_MOVE
    hash(hash&&) = default;
#else
    hash(hash&&) {}
#endif
    hash& operator=(const hash&) = default;
#ifdef SPSL_HAS_DEFAULT_MOVE
    hash& operator=(hash&&) = default;
#else
    hash& operator=(hash&&) { return *this; }
#endif
    ~hash() = default;

    size_t operator()(const argument_type& s) const
    {
        using char_type = typename spsl::StringBase<StorageType>::char_type;
        return spsl::hash::hash_impl(s.data(), s.size() * sizeof(char_type));
    }
};
}

#endif /* SPSL_STRINGBASE_HPP_ */
