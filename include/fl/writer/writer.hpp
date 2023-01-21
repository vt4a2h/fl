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

#include <functional>
#include <concepts>

#include "fl/semigroups/semigroup.hpp"
#include "fl/monoids/monoid.hpp"
#include "fl/concepts/concepts.hpp"
#include "fl/util/any_semigroup.hpp"
#include "fl/util/move_if_possible.hpp"

namespace fl {

template<class LogType, class ValueType>
struct Writer;

template<class LogType, class ValueType>
Writer(LogType &&l, ValueType &&v) -> Writer<std::remove_cvref_t<LogType>, std::remove_cvref_t<ValueType>>;

namespace _details {
    template <class L, class V>
    constexpr auto make_writer(L &&l, V &&v) { return Writer{std::forward<L>(l), std::forward<V>(v)}; }
}

namespace _concepts {
template <class V>
concept WithSemigroup = requires { Semigroup<V>(); };

template <class V>
concept WithMonoid = requires { Monoid<V>(); };

template <class... Args>
concept MoveConstructable = (std::is_move_constructible_v<Args> && ...);
}

template<class Log, class Value>
struct Writer {
    using LogType = std::remove_cvref_t<Log>;
    using ValueType = std::remove_cvref_t<Value>;
    using writer_tag_ = Writer<LogType, ValueType>;

    Writer() = default;
    Writer(const Writer &) = default;
    Writer(Writer &&) noexcept = default;

    Writer &operator=(const Writer &) = default;
    Writer &operator=(Writer &&) noexcept = default;

    Writer(const Log &l, const Value &v) : _log(l), _value(v) {}

    Writer(Log &&l, Value &&v)
        requires _concepts::MoveConstructable<LogType, ValueType>
            : _log(std::move(l)), _value(std::move(v)) {}

    Writer(Log &&l, const Value &v)
        requires _concepts::MoveConstructable<LogType>
            : _log(std::move(l)), _value(v) {}

    Writer(const Log &l, Value &&v)
        requires _concepts::MoveConstructable<ValueType>
            : _log(l), _value(std::move(v)) {}

    constexpr auto transform(concepts::Invocable<ValueType> auto f) const & {
        return _details::make_writer(_log, std::invoke(f, _value));
    }

    constexpr auto transform(concepts::Invocable<ValueType> auto f) && {
        return _details::make_writer(fl::util::move_if_possible(_log), std::invoke(f, fl::util::move_if_possible(_value)));
    }

    constexpr auto transform(concepts::Invocable<ValueType> auto f) const && {
        return _details::make_writer(fl::util::move_if_possible(_log), std::invoke(f, fl::util::move_if_possible(_value)));
    }

    constexpr auto tell(LogType &&l) const &
    requires _concepts::WithSemigroup<LogType> {
        return _details::make_writer(Semigroup<LogType>{}.combine(_log, fl::util::move_if_possible(l)), _value);
    }

    constexpr auto tell(const LogType &l) const &
    requires _concepts::WithSemigroup<LogType> {
        return _details::make_writer(Semigroup<LogType>{}.combine(_log, l), _value);
    }

    constexpr auto tell(LogType &&l) &&
    requires _concepts::WithSemigroup<LogType> {
        return _details::make_writer(
            Semigroup<LogType>{}.combine(fl::util::move_if_possible(_log), fl::util::move_if_possible(l)),
            fl::util::move_if_possible(_value)
        );
    }

    constexpr auto tell(const LogType &l) &&
    requires _concepts::WithSemigroup<LogType> {
        return _details::make_writer(Semigroup<LogType>{}.combine(fl::util::move_if_possible(_log), l),
                                     fl::util::move_if_possible(_value));
    }

    constexpr auto tell(LogType &&l) const &&
    requires _concepts::WithSemigroup<LogType> {
        return _details::make_writer(
            Semigroup<LogType>{}.combine(fl::util::move_if_possible(_log), fl::util::move_if_possible(l)),
            fl::util::move_if_possible(_value)
        );
    }

    constexpr auto tell(const LogType &l) const &&
    requires _concepts::WithSemigroup<LogType> {
        return _details::make_writer(Semigroup<LogType>{}.combine(fl::util::move_if_possible(_log), l),
                                     fl::util::move_if_possible(_value));
    }

    constexpr auto tell(const LogType &l, const SemigroupWrapper<LogType> &sg) const & {
        return _details::make_writer(sg.combine(_log, l), _value);
    }

    constexpr auto tell(LogType &&l, const SemigroupWrapper<LogType> &sg) const & {
        return _details::make_writer(sg.combine(_log, fl::util::move_if_possible(l)), _value);
    }

    constexpr auto tell(const LogType &l, const SemigroupWrapper<LogType> &sg) && {
        return _details::make_writer(sg.combine(fl::util::move_if_possible(_log), l), fl::util::move_if_possible(_value));
    }

    constexpr auto tell(LogType &&l, const SemigroupWrapper<LogType> &sg) && {
        return _details::make_writer(sg.combine(fl::util::move_if_possible(_log), fl::util::move_if_possible(l)),
                                     fl::util::move_if_possible(_value));
    }

    constexpr auto tell(const LogType &l, const SemigroupWrapper<LogType> &sg) const && {
        return _details::make_writer(sg.combine(fl::util::move_if_possible(_log), l), fl::util::move_if_possible(_value));
    }

    constexpr auto tell(LogType &&l, const SemigroupWrapper<LogType> &sg) const && {
        return _details::make_writer(sg.combine(fl::util::move_if_possible(_log), fl::util::move_if_possible(l)),
                                     fl::util::move_if_possible(_value));
    }

    constexpr auto swap() const & {
        return _details::make_writer(_value, _log);
    }

    constexpr auto swap() && {
        return _details::make_writer(fl::util::move_if_possible(_value), fl::util::move_if_possible(_log));
    }

    constexpr auto swap() const && {
        return _details::make_writer(fl::util::move_if_possible(_value), fl::util::move_if_possible(_log));
    }

    constexpr auto value() const & {
        return _value;
    }

    constexpr auto value() && {
        return fl::util::move_if_possible(_value);
    }

    constexpr auto value() const && {
        return fl::util::move_if_possible(_value);
    }

    constexpr auto log() const & {
        return _log;
    }

    constexpr auto log() && {
        return fl::util::move_if_possible(_log);
    }

    constexpr auto log() const && {
        return fl::util::move_if_possible(_log);
    }

    constexpr auto as_tuple() const & {
        return std::make_tuple(_log, _value);
    }

    constexpr auto as_tuple() && {
        return std::make_tuple(fl::util::move_if_possible(_log), fl::util::move_if_possible(_value));
    }

    constexpr auto as_tuple() const && {
        return std::make_tuple(fl::util::move_if_possible(_log), fl::util::move_if_possible(_value));
    }

    constexpr auto reset() const
    requires _concepts::WithMonoid<LogType> {
        return _details::make_writer(Monoid<LogType>().identity(), _value);
    }

    constexpr auto and_then(concepts::InvocableAndReturnsWriter<ValueType> auto f) const &
    requires _concepts::WithSemigroup<LogType> {
        auto &&w = std::invoke(f, _value);
        return _details::make_writer(
            Semigroup<LogType>().combine(_log, fl::util::move_if_possible(w._log)),
            fl::util::move_if_possible(w._value)
        );
    }

    constexpr auto and_then(concepts::InvocableAndReturnsWriter<ValueType> auto f) &&
    requires _concepts::WithSemigroup<LogType> {
        auto &&w = std::invoke(f, fl::util::move_if_possible(_value));
        return _details::make_writer(
            Semigroup<LogType>().combine(fl::util::move_if_possible(_log), fl::util::move_if_possible(w._log)),
            fl::util::move_if_possible(w._value)
        );
    }

    constexpr auto and_then(concepts::InvocableAndReturnsWriter<ValueType> auto f) const &&
    requires _concepts::WithSemigroup<LogType> {
        auto &&w = std::invoke(f, fl::util::move_if_possible(_value));
        return _details::make_writer(
            Semigroup<LogType>().combine(fl::util::move_if_possible(_log), fl::util::move_if_possible(w._log)),
            fl::util::move_if_possible(w._value)
        );
    }

    template<class FunctionLikeValue>
    constexpr auto apply(const Writer<LogType, FunctionLikeValue> &w) const &
    requires _concepts::WithSemigroup<LogType> {
        return _details::make_writer(Semigroup<LogType>().combine(w._log, _log), std::invoke(w._value, _value));
    }

    template<class FunctionLikeValue>
    constexpr auto apply(Writer<LogType, FunctionLikeValue> &&w) const &
    requires _concepts::WithSemigroup<LogType> {
        return _details::make_writer(Semigroup<LogType>().combine(fl::util::move_if_possible(w._log), _log),
                                     std::invoke(fl::util::move_if_possible(w._value), _value));
    }

    template<class FunctionLikeValue>
    constexpr auto apply(Writer<LogType, FunctionLikeValue> &&w) &&
    requires _concepts::WithSemigroup<LogType> {
        return _details::make_writer(Semigroup<LogType>().combine(fl::util::move_if_possible(w._log), fl::util::move_if_possible(_log)),
                                     std::invoke(fl::util::move_if_possible(w._value), fl::util::move_if_possible(_value)));
    }

    template<class FunctionLikeValue>
    constexpr auto apply(Writer<LogType, FunctionLikeValue> &&w) const &&
    requires _concepts::WithSemigroup<LogType> {
        return _details::make_writer(Semigroup<LogType>().combine(fl::util::move_if_possible(w._log), fl::util::move_if_possible(_log)),
                                     std::invoke(fl::util::move_if_possible(w._value), fl::util::move_if_possible(_value)));
    }

    template<class FunctionLikeValue>
    constexpr auto apply(const Writer<LogType, FunctionLikeValue> &w) &&
    requires _concepts::WithSemigroup<LogType> {
        return _details::make_writer(Semigroup<LogType>().combine(w._log, fl::util::move_if_possible(_log)),
                                     std::invoke(w._value, fl::util::move_if_possible(_value)));
    }

    template<class FunctionLikeValue>
    constexpr auto apply(const Writer<LogType, FunctionLikeValue> &w) const &&
    requires _concepts::WithSemigroup<LogType> {
        return _details::make_writer(Semigroup<LogType>().combine(w._log, fl::util::move_if_possible(_log)),
                                     std::invoke(w._value, fl::util::move_if_possible(_value)));
    }

    LogType _log;
    ValueType _value;
};

} // namespace fl
