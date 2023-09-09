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

} // fl::concepts
