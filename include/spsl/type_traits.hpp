/**
 * @file    Special Purpose Strings Library: type_traits.hpp
 * @author  Daniel Evers
 * @brief   Type traits
 * @license MIT
 */

#ifndef SPSL_TYPE_TRAITS_HPP_
#define SPSL_TYPE_TRAITS_HPP_

#include <iterator>
#include <type_traits>

namespace spsl
{

/**
 * Checks whether a given type satisfies the InputIterator requirements.
 */
template <typename Iter>
using checkInputIter = typename std::enable_if_t<std::is_convertible_v<
  typename std::iterator_traits<Iter>::iterator_category, std::input_iterator_tag>>;

} // namespace spsl

#endif /* SPSL_TYPE_TRAITS_HPP_ */
