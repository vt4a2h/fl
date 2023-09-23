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

/*!
 * Writer "monad".
 *
 * This abstraction can be used for logging without side effects. It's also possible to compose functions that return
 * Writer<Log, Value>. For example:
 * \code{.cpp}
 *    using Log = std::vector<std::string>;
 *    using Value = std::uint64_t;
 *    using Logger = fl::Writer<Log, Value>;
 *
 *    [[nodiscard]]
 *    Logger factorial(Val i) {
 *        const auto mult = [&](Val v) { return v * i; };
 *        const auto tell = [&](Val ans) { return Logger{{fmt::format("Factorial of {} is {}", i, ans)}, ans}; };
 *        return (i == 0 ? Logger{{}, 1} : factorial(i - 1).transform(mult)).and_then(tell);
 *    }
 * \endcode
 *
 * @tparam Log the type of log.
 * @tparam Value the type of value.
 */
template<class Log, class Value>
struct Writer {
    using LogType = std::remove_cvref_t<Log>;
    using ValueType = std::remove_cvref_t<Value>;

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

    /*!
     * Transform the existing value of writer.
     *
     * @param f function to transform the value.
     * @return a copy of the object with the same log, but transformed value.
     */
    constexpr auto transform(concepts::Invocable<ValueType> auto f) const & {
        return _details::make_writer(_log, std::invoke(f, _value));
    }

    constexpr auto transform(concepts::Invocable<ValueType> auto f) && {
        return _details::make_writer(fl::util::move_if_possible(_log), std::invoke(f, fl::util::move_if_possible(_value)));
    }

    constexpr auto transform(concepts::Invocable<ValueType> auto f) const && {
        return _details::make_writer(fl::util::move_if_possible(_log), std::invoke(f, fl::util::move_if_possible(_value)));
    }

    /*!
     * Add a log entry.
     *
     * The class \p Semigroup is used for combining the existing log with the new one. This class must exist for
     * \p LogType.
     *
     * @param l a log entry to add.
     * @return a copy of the object with the same value and combined logs.
     */
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

    template<class LogEntry>
    requires _concepts::WithSemigroup<LogType> &&
              concepts::CombinableWithValue<Semigroup<LogType>, LogType, std::remove_cvref_t<LogEntry>>
    constexpr auto tell(LogEntry &&l) const & {
        return _details::make_writer(Semigroup<LogType>{}.combine(_log, std::forward<LogEntry>(l)), _value);
    }

    template<class LogEntry>
    requires _concepts::WithSemigroup<LogType> &&
             concepts::CombinableWithValue<Semigroup<LogType>, LogType, std::remove_cvref_t<LogEntry>>
    constexpr auto tell(LogEntry &&l) && {
        return _details::make_writer(Semigroup<LogType>{}.combine(std::move(_log), std::forward<LogEntry>(l)),
                                     fl::util::move_if_possible(_value));
    }

    /*!
     * Change places of value and log.
     *
     * @return a new writer object of the type Writer<Value, Log>.
     */
    constexpr auto swap() const & {
        return _details::make_writer(_value, _log);
    }

    constexpr auto swap() && {
        return _details::make_writer(fl::util::move_if_possible(_value), fl::util::move_if_possible(_log));
    }

    constexpr auto swap() const && {
        return _details::make_writer(fl::util::move_if_possible(_value), fl::util::move_if_possible(_log));
    }

    /*!
     * Get value.
     * @return a copy of the value object.
     */
    constexpr auto value() const & {
        return _value;
    }

    constexpr auto value() && {
        return fl::util::move_if_possible(_value);
    }

    constexpr auto value() const && {
        return fl::util::move_if_possible(_value);
    }

    /*!
     * Get log.
     * @return a copy of the log object.
     */
    constexpr auto log() const & {
        return _log;
    }

    constexpr auto log() && {
        return fl::util::move_if_possible(_log);
    }

    constexpr auto log() const && {
        return fl::util::move_if_possible(_log);
    }

    /*!
     * Tuple of log and value objects.
     * @return a tuple of copies of log and value objects.
     */
    constexpr auto as_tuple() const & {
        return std::make_tuple(_log, _value);
    }

    constexpr auto as_tuple() && {
        return std::make_tuple(fl::util::move_if_possible(_log), fl::util::move_if_possible(_value));
    }

    constexpr auto as_tuple() const && {
        return std::make_tuple(fl::util::move_if_possible(_log), fl::util::move_if_possible(_value));
    }

    /*!
     * Drop existing logs.
     *
     * The class \p Monoid must exist for the \p LogType.
     *
     * @return a new object with empty log and the same value.
     */
    constexpr auto reset() const
    requires _concepts::WithMonoid<LogType> {
        return _details::make_writer(Monoid<LogType>().identity(), _value);
    }

    /*!
     * Compose this writer with the function \p f.
     * @param f function that accepts Writer object of this type. The function must also return a writer.
     * @return a new writer object.
     */
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

    /*!
     * Compose this writer with another writer.
     *
     * The writer \w must have an invokable value type. The value will be composed with this writer's value.
     * The class Semigroup must be available for the \p LogType.
     *
     * @param w another writer to compose.
     * @return a new writer contains combined logs and composed values.
     */
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

    auto operator<=> (const Writer&) const = default;
    bool operator== (const Writer&) const = default;

    LogType _log;
    ValueType _value;
};

} // namespace fl
