//
// MIT License
//
// Copyright (c) 2022-present Vitaly Fanaskov
//
// fl -- Writer "monad" for C++
// Project home: https://github.com/vt4a2h/fl
//
// See LICENSE file for the further details.
//
#pragma once

#include <concepts>
#include <utility>

namespace fl {

template<class Log, class Value>
struct Writer;

} // namespace fl

namespace fl::concepts {

template<class F, class Arg>
concept Invocable = std::is_invocable_v<F, Arg>;

template<class F, class Arg, class Result>
concept InvocableWithResult = std::is_invocable_r_v<Result, F, Arg>;

template<class S, class V = typename std::remove_cvref_t<S>::ValueType>
concept IsProbablySemigroup = requires(S s, V v) {
    typename std::remove_cvref_t<S>::ValueType;

    { s.combine(v, v) } -> std::same_as<V>;
    { s.combine(std::move(v), v) } -> std::same_as<V>;
    { s.combine(v, std::move(v)) } -> std::same_as<V>;
    { s.combine(std::move(v), std::move(v)) } -> std::same_as<V>;
};

template <class W>
concept IsWriter = std::same_as<
    std::remove_cvref_t<W>,
    fl::Writer<typename std::remove_cvref_t<W>::LogType, typename std::remove_cvref_t<W>::ValueType>
>;

template<class F, class Arg>
concept InvocableAndReturnsWriter =
    std::is_invocable_v<F, Arg> && IsWriter<std::invoke_result_t<F, Arg>>;

template <class C>
concept StdContainer = requires(C c) {
    typename C::value_type;
    typename C::size_type;
    typename C::reference;
    typename C::const_reference;
    typename C::iterator;
    typename C::const_iterator;
    typename C::difference_type;

    requires std::default_initializable<C>;
    requires std::move_constructible<C>;
    requires std::copy_constructible<C>;
    requires std::destructible<C>;
    { c.begin() } -> std::same_as<typename C::iterator>;
    { c.end() } -> std::same_as<typename C::iterator>;
    { c.cbegin() } -> std::same_as<typename C::const_iterator>;
    { c.cend() } -> std::same_as<typename C::const_iterator>;
    requires std::equality_comparable<C>;
    requires std::swappable<C>;
    { c.size() } -> std::same_as<typename C::size_type>;
    { c.max_size() } -> std::same_as<typename C::size_type>;
    { c.empty() } -> std::convertible_to<bool>;
};

template<class Container>
concept PushableContainer =
StdContainer<Container> &&
    requires(Container c, typename Container::value_type &&v) {
        c.reserve(typename Container::size_type{});
        c.push_back(v);
    };

template<class Container>
concept InsertableContainer =
StdContainer<Container> &&
    requires(Container c, typename Container::value_type v, typename Container::iterator it) {
        c.insert(v);
        c.insert(it, it);
    };

} // fl::concepts
