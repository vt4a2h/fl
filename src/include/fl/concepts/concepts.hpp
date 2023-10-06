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

template<class V>
struct Semigroup;

template<class V>
struct SemigroupWrapper;

} // namespace fl

namespace fl::concepts {

template <class Arg, class Type>
concept SameOrConstructable =
    std::is_same_v<std::remove_cvref_t<Arg>, std::remove_cvref_t<Type>> ||
    std::is_constructible_v<std::remove_cvref_t<Type>, std::remove_cvref_t<Arg>>;

namespace details {

template<class S, class Container, class Value>
concept CombinableWithValue = requires(S s, Container &&v1, Value &&v2) {
    { s.combine(std::forward<Container>(v1), std::forward<Value>(v2)) } -> std::same_as<std::remove_cvref_t<Container>>;
};

template <class TellEntry, class WriterLogType, class Sg>
concept CombinableWithSg =
    CombinableWithValue<Sg, WriterLogType, std::remove_cvref_t<TellEntry>> || SameOrConstructable<TellEntry, WriterLogType>;

} // details

template<class F, class Arg>
concept Invocable = std::is_invocable_v<F, Arg>;

template<class F, class Arg, class Result>
concept InvocableWithResult = std::is_invocable_r_v<Result, F, Arg>;

template<class S, class V = typename std::remove_cvref_t<S>::ValueType>
concept IsProbablySemigroup = requires(S s, V &&v1, V &&v2) {
    typename std::remove_cvref_t<S>::ValueType;

    { s.combine(std::forward<V>(v1), std::forward<V>(v2)) } -> std::same_as<std::remove_cvref_t<V>>;
};

template <class V>
concept WithSemigroup = requires { Semigroup<V>(); };

template <class TellEntry, class WriterLogType>
concept ValidTellEntry = WithSemigroup<WriterLogType> && details::CombinableWithSg<TellEntry, WriterLogType, Semigroup<WriterLogType>>;

template <class TellEntry, class WriterLogType>
concept CombinableWithSgWrapper = details::CombinableWithSg<TellEntry, WriterLogType, SemigroupWrapper<WriterLogType>>;

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
StdContainer<std::remove_cvref_t<Container>> &&
    requires(std::remove_cvref_t<Container> c, typename std::remove_cvref_t<Container>::value_type &&v) {
        c.reserve(typename std::remove_cvref_t<Container>::size_type{});
        c.push_back(v);
    };

template<class Container>
concept InsertableContainer =
StdContainer<std::remove_cvref_t<Container>> &&
    requires(std::remove_cvref_t<Container> c, typename std::remove_cvref_t<Container>::value_type v, typename std::remove_cvref_t<Container>::iterator it) {
        c.insert(v);
        c.insert(it, it);
    };

template<class Container>
concept PushableOrInsertableContainer = PushableContainer<Container> || InsertableContainer<Container>;

template<class FirstContainer, class SecondContainer>
concept SameContainer = std::is_same_v<std::remove_cvref_t<FirstContainer>, std::remove_cvref_t<SecondContainer>>;

template<class Value, class Container>
concept SameElementType =
    std::is_same_v<typename std::remove_cvref_t<Container>::value_type, std::remove_cvref_t<Value>> ||
        std::is_constructible_v<typename std::remove_cvref_t<Container>::value_type, std::remove_cvref_t<Value>>;

} // fl::concepts
