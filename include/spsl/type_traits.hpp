/**
 * @file    Special Purpose Strings Library: type_traits.hpp
 * @author  Daniel Evers
 * @brief   Type traits
 * @license MIT
 */

#ifndef SPSL_TYPE_TRAITS_HPP_
#define SPSL_TYPE_TRAITS_HPP_

#include <type_traits>

namespace spsl
{

/*
 * Simple templates to determine if a class is a "compatible" string class. We require that
 * there is a @c data() method that returns a (const) character pointer and a @c size() method
 * that returns the length.
 * This works for std::basic_string, std::basic_string_view, std::vector,
 * spsl::StringCore/StringBase and hopefully for a lot of other similar classes.
 *
 * (std::initializer_list unfortunately used begin() + size() ...)
 */


// check if data() returns (const) CharType*
template <typename CharType, typename Class>
struct has_data
{
private:
    /*
     * We are checking if (const T).data() returns const CharType*. Removing const to turn
     * const CharType* into CharType* is more complicated (std::remove_const doesn't do the
     * trick), so let's assume that a const T won't return a pointer to a writeable location.
     */
    template <typename T>
    static constexpr auto check(T*) ->
      typename std::is_same<CharType const*, decltype(std::declval<const T>().data())>::type
    {
        return {};
    }

    template <typename>
    static constexpr std::false_type check(...)
    {
        return {};
    }

public:
    using type = decltype(check<Class>(nullptr));
    static constexpr bool value = type::value;
};

// check if size() returns SizeType
template <typename SizeType, typename Class>
struct has_size
{
private:
    template <typename T>
    static constexpr auto check(T*) ->
      typename std::is_same<SizeType, decltype(std::declval<T>().size())>::type
    {
        return {};
    }

    template <typename>
    static constexpr std::false_type check(...)
    {
        return {};
    }

public:
    using type = decltype(check<Class>(nullptr));
    static constexpr bool value = type::value;
};


/**
 * Checks whether a string class has matching data() and size() methods.
 */
template <typename CharType, typename SizeType, typename Class>
struct is_compatible_string
  : public std::integral_constant<bool, has_data<CharType, Class>::value &&
                                          has_size<SizeType, Class>::value>
{
};

template <typename MyString, typename Class>
struct is_compatible_string2
  : public std::integral_constant<bool, has_data<typename MyString::char_type, Class>::value &&
                                          has_size<typename MyString::size_type, Class>::value>
{
};

/**
 * Checks whether a given type satisfies the InputIterator requirements.
 */
template <typename Iter>
using checkInputIter = typename std::enable_if<std::is_convertible<
  typename std::iterator_traits<Iter>::iterator_category, std::input_iterator_tag>::value>::type;
}

#endif /* SPSL_TYPE_TRAITS_HPP_ */
