//
// Copyright (C) 2014, 2015 Ableton AG, Berlin. All rights reserved.
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//

/*!
 * @file
 */

#pragma once

#include <ableton/build_system/Warnings.hpp>
ABL_DISABLE_WARNINGS
#include <boost/mpl/sequence_tag.hpp>
#include <boost/mpl/pop_front_fwd.hpp>
#include <boost/mpl/push_front_fwd.hpp>
#include <boost/mpl/push_back_fwd.hpp>
#include <boost/mpl/front_fwd.hpp>
#include <boost/mpl/empty_fwd.hpp>
#include <boost/mpl/size_fwd.hpp>
#include <boost/mpl/at_fwd.hpp>
#include <boost/mpl/back_fwd.hpp>
#include <boost/mpl/clear_fwd.hpp>
#include <boost/mpl/pop_back_fwd.hpp>
#include <boost/mpl/iterator_tags.hpp>
#include <boost/mpl/next_prior.hpp>
#include <boost/mpl/deref.hpp>
#include <boost/mpl/begin_end_fwd.hpp>
ABL_RESTORE_WARNINGS
#include <type_traits>

namespace atria {
namespace meta {

/*!
 * MPL-compatible sequence that just holds a vector of types as a
 * paremeter pack.
 */
template <typename ...Ts>
struct pack
{
};

/*!
 * Two packs are equal if they are of the same type.
 */
template <typename... Ts1, typename... Ts2>
constexpr bool operator== (const pack<Ts1...>&, const pack<Ts2...>&)
{
  return std::is_same<pack<Ts1...>, pack<Ts2...> >{};
}

/*!
 * Two packs are different if they are of different types.
 */
template <typename... Ts1, typename... Ts2>
constexpr bool operator!= (const pack<Ts1...>&, const pack<Ts2...>&)
{
  return !std::is_same<pack<Ts1...>, pack<Ts2...> >{};
}

namespace detail {

template <template<typename...> class MF, typename ArgT>
struct unpack
{
  using type = MF<ArgT>;
};

template <template<typename...> class MF, typename... ArgTs>
struct unpack<MF, meta::pack<ArgTs...> >
{
  using type = MF<ArgTs...>;
};

} // namespace detail

/*!
 * Metafunction that given a variadic template `MF` and a type `ArgT`,
 * returns `MF<ArgT>`, or if ArgT is of the form `pack<Args...>` then
 * returns `MF<Args...>`.
 */
template <template<typename...> class MF, typename T>
using unpack = typename detail::unpack<MF, T>::type;

template <template<typename...> class MF, typename T>
using unpack_t = typename unpack<MF, T>::type;

struct pack_tag;

template <class ... Args>
struct pack_iterator;

template <class ... Args>
struct pack_iterator<atria::meta::pack<Args...>>
{
  typedef atria::meta::pack_tag tag;
  typedef boost::mpl::forward_iterator_tag category;
};

} // namespace meta
} // namespace atria

namespace boost {
namespace mpl {

template <class ... Args>
struct sequence_tag<atria::meta::pack<Args...>>
{
  typedef atria::meta::pack_tag type;
};

template <>
struct front_impl<atria::meta::pack_tag>
{
  template <typename PackT> struct apply;
  template <typename T, typename ...Ts>
  struct apply<atria::meta::pack<T, Ts...>>
  {
    using type = T;
  };
};

template <>
struct empty_impl<atria::meta::pack_tag>
{
  template <typename PackT> struct apply;
  template <typename ...Ts>
  struct apply<atria::meta::pack<Ts...>>
    : std::integral_constant<bool, sizeof...(Ts) == 0>
  {};
};

template <>
struct pop_front_impl<atria::meta::pack_tag>
{
  template <typename PackT> struct apply;
  template <class First, class ...Types>
  struct apply<atria::meta::pack<First, Types...>>
  {
    typedef atria::meta::pack<Types...> type;
  };
};

template <>
struct push_front_impl<atria::meta::pack_tag>
{
  template <typename PackT, typename T> struct apply;
  template <typename T, typename ... Args>
  struct apply<atria::meta::pack<Args...>, T>
  {
    typedef atria::meta::pack<T, Args...> type;
  };
};

template <>
struct push_back_impl<atria::meta::pack_tag>
{
  template <typename PackT, typename T> struct apply;
  template <typename T, typename ... Args >
  struct apply<atria::meta::pack<Args...>, T>
  {
    typedef atria::meta::pack<Args..., T> type;
  };
};

template <>
struct size_impl<atria::meta::pack_tag>
{
  template <typename PackT> struct apply;
  template <typename ...Ts>
  struct apply<atria::meta::pack<Ts...>>
    : std::integral_constant<std::size_t, sizeof...(Ts)>
  {};
};

template <>
struct at_impl<atria::meta::pack_tag>
{
  template <typename T>
  struct apply
  {
    static_assert(std::is_same<T, T>::value, "not implemented");
  };
};

template <>
struct back_impl<atria::meta::pack_tag>
{
  template <typename T>
  struct apply
  {
    static_assert(std::is_same<T, T>::value, "not implemented");
  };
};

template <>
struct clear_impl<atria::meta::pack_tag>
{
  template <typename PackT>
  struct apply
  {
    typedef atria::meta::pack<> type;
  };
};

template <>
struct pop_back_impl<atria::meta::pack_tag>
{
  template <typename T>
  struct apply
  {
    static_assert(std::is_same<T, T>::value, "not implemented");
  };
};

template <>
struct begin_impl<atria::meta::pack_tag>
{
  template <class PackT> struct apply
  {
    typedef atria::meta::pack_iterator<PackT> type;
  };
};

template <>
struct end_impl<atria::meta::pack_tag>
{
  template <typename> struct apply
  {
    typedef atria::meta::pack_iterator<
      atria::meta::pack<>> type;
  };
};

template <typename First, class ... Args>
struct deref<atria::meta::pack_iterator<
  atria::meta::pack<First, Args...>>>
{
  typedef First type;
};

template <typename First, class ... Args>
struct next<atria::meta::pack_iterator<
  atria::meta::pack<First, Args...>>>
{
  typedef atria::meta::pack_iterator<
    atria::meta::pack<Args...>> type;
};

} // namespace mpl
} // namespace boost
