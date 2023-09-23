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

    Writer(const Log &l, const Value &v) : log_(l), value_(v) {}

    Writer(Log &&l, Value &&v)
        requires _concepts::MoveConstructable<LogType, ValueType>
            : log_(std::move(l)), value_(std::move(v)) {}

    Writer(Log &&l, const Value &v)
        requires _concepts::MoveConstructable<LogType>
            : log_(std::move(l)), value_(v) {}

    Writer(const Log &l, Value &&v)
        requires _concepts::MoveConstructable<ValueType>
            : log_(l), value_(std::move(v)) {}

    /*!
     * Transform the existing value of writer.
     *
     * @param f function to transform the value.
     * @return a copy of the object with the same log, but transformed value.
     */
    constexpr auto transform(concepts::Invocable<ValueType> auto f) const & {
        return _details::make_writer(log_, std::invoke(f, value_));
    }

    constexpr auto transform(concepts::Invocable<ValueType> auto f) && {
        return _details::make_writer(fl::util::move_if_possible(log_), std::invoke(f, fl::util::move_if_possible(value_)));
    }

    constexpr auto transform(concepts::Invocable<ValueType> auto f) const && {
        return _details::make_writer(fl::util::move_if_possible(log_), std::invoke(f, fl::util::move_if_possible(value_)));
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
        return _details::make_writer(Semigroup<LogType>{}.combine(log_, fl::util::move_if_possible(l)), value_);
    }

    constexpr auto tell(const LogType &l) const &
    requires _concepts::WithSemigroup<LogType> {
        return _details::make_writer(Semigroup<LogType>{}.combine(log_, l), value_);
    }

    constexpr auto tell(LogType &&l) &&
    requires _concepts::WithSemigroup<LogType> {
        return _details::make_writer(
            Semigroup<LogType>{}.combine(fl::util::move_if_possible(log_), fl::util::move_if_possible(l)),
            fl::util::move_if_possible(value_)
        );
    }

    constexpr auto tell(const LogType &l) &&
    requires _concepts::WithSemigroup<LogType> {
        return _details::make_writer(Semigroup<LogType>{}.combine(fl::util::move_if_possible(log_), l),
                                     fl::util::move_if_possible(value_));
    }

    constexpr auto tell(LogType &&l) const &&
    requires _concepts::WithSemigroup<LogType> {
        return _details::make_writer(
            Semigroup<LogType>{}.combine(fl::util::move_if_possible(log_), fl::util::move_if_possible(l)),
            fl::util::move_if_possible(value_)
        );
    }

    constexpr auto tell(const LogType &l) const &&
    requires _concepts::WithSemigroup<LogType> {
        return _details::make_writer(Semigroup<LogType>{}.combine(fl::util::move_if_possible(log_), l),
                                     fl::util::move_if_possible(value_));
    }

    constexpr auto tell(const LogType &l, const SemigroupWrapper<LogType> &sg) const & {
        return _details::make_writer(sg.combine(log_, l), value_);
    }

    constexpr auto tell(LogType &&l, const SemigroupWrapper<LogType> &sg) const & {
        return _details::make_writer(sg.combine(log_, fl::util::move_if_possible(l)), value_);
    }

    constexpr auto tell(const LogType &l, const SemigroupWrapper<LogType> &sg) && {
        return _details::make_writer(sg.combine(fl::util::move_if_possible(log_), l), fl::util::move_if_possible(value_));
    }

    constexpr auto tell(LogType &&l, const SemigroupWrapper<LogType> &sg) && {
        return _details::make_writer(sg.combine(fl::util::move_if_possible(log_), fl::util::move_if_possible(l)),
                                     fl::util::move_if_possible(value_));
    }

    constexpr auto tell(const LogType &l, const SemigroupWrapper<LogType> &sg) const && {
        return _details::make_writer(sg.combine(fl::util::move_if_possible(log_), l), fl::util::move_if_possible(value_));
    }

    constexpr auto tell(LogType &&l, const SemigroupWrapper<LogType> &sg) const && {
        return _details::make_writer(sg.combine(fl::util::move_if_possible(log_), fl::util::move_if_possible(l)),
                                     fl::util::move_if_possible(value_));
    }

    template<class LogEntry>
    requires _concepts::WithSemigroup<LogType> &&
              concepts::CombinableWithValue<Semigroup<LogType>, LogType, std::remove_cvref_t<LogEntry>>
    constexpr auto tell(LogEntry &&l) const & {
        return _details::make_writer(Semigroup<LogType>{}.combine(log_, std::forward<LogEntry>(l)), value_);
    }

    template<class LogEntry>
    requires _concepts::WithSemigroup<LogType> &&
             concepts::CombinableWithValue<Semigroup<LogType>, LogType, std::remove_cvref_t<LogEntry>>
    constexpr auto tell(LogEntry &&l) && {
        return _details::make_writer(Semigroup<LogType>{}.combine(std::move(log_), std::forward<LogEntry>(l)),
                                     fl::util::move_if_possible(value_));
    }

    /*!
     * Change places of value and log.
     *
     * @return a new writer object of the type Writer<Value, Log>.
     */
    constexpr auto swap() const & {
        return _details::make_writer(value_, log_);
    }

    constexpr auto swap() && {
        return _details::make_writer(fl::util::move_if_possible(value_), fl::util::move_if_possible(log_));
    }

    constexpr auto swap() const && {
        return _details::make_writer(fl::util::move_if_possible(value_), fl::util::move_if_possible(log_));
    }

    /*!
     * Get value.
     * @return a copy of the value object.
     */
    constexpr auto value() const & {
        return value_;
    }

    constexpr auto value() && {
        return fl::util::move_if_possible(value_);
    }

    constexpr auto value() const && {
        return fl::util::move_if_possible(value_);
    }

    /*!
     * Get log.
     * @return a copy of the log object.
     */
    constexpr auto log() const & {
        return log_;
    }

    constexpr auto log() && {
        return fl::util::move_if_possible(log_);
    }

    constexpr auto log() const && {
        return fl::util::move_if_possible(log_);
    }

    /*!
     * Tuple of log and value objects.
     * @return a tuple of copies of log and value objects.
     */
    constexpr auto as_tuple() const & {
        return std::make_tuple(log_, value_);
    }

    constexpr auto as_tuple() && {
        return std::make_tuple(fl::util::move_if_possible(log_), fl::util::move_if_possible(value_));
    }

    constexpr auto as_tuple() const && {
        return std::make_tuple(fl::util::move_if_possible(log_), fl::util::move_if_possible(value_));
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
        return _details::make_writer(Monoid<LogType>().identity(), value_);
    }

    /*!
     * Compose this writer with the function \p f.
     * @param f function that accepts Writer object of this type. The function must also return a writer.
     * @return a new writer object.
     */
    constexpr auto and_then(concepts::InvocableAndReturnsWriter<ValueType> auto f) const &
    requires _concepts::WithSemigroup<LogType> {
        auto &&w = std::invoke(f, value_);
        return _details::make_writer(
            Semigroup<LogType>().combine(log_, fl::util::move_if_possible(w.log_)),
            fl::util::move_if_possible(w.value_)
        );
    }

    constexpr auto and_then(concepts::InvocableAndReturnsWriter<ValueType> auto f) &&
    requires _concepts::WithSemigroup<LogType> {
        auto &&w = std::invoke(f, fl::util::move_if_possible(value_));
        return _details::make_writer(
            Semigroup<LogType>().combine(fl::util::move_if_possible(log_), fl::util::move_if_possible(w.log_)),
            fl::util::move_if_possible(w.value_)
        );
    }

    constexpr auto and_then(concepts::InvocableAndReturnsWriter<ValueType> auto f) const &&
    requires _concepts::WithSemigroup<LogType> {
        auto &&w = std::invoke(f, fl::util::move_if_possible(value_));
        return _details::make_writer(
            Semigroup<LogType>().combine(fl::util::move_if_possible(log_), fl::util::move_if_possible(w.log_)),
            fl::util::move_if_possible(w.value_)
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
        return _details::make_writer(Semigroup<LogType>().combine(w.log_, log_), std::invoke(w.value_, value_));
    }

    template<class FunctionLikeValue>
    constexpr auto apply(Writer<LogType, FunctionLikeValue> &&w) const &
    requires _concepts::WithSemigroup<LogType> {
        return _details::make_writer(Semigroup<LogType>().combine(fl::util::move_if_possible(w.log_), log_),
                                     std::invoke(fl::util::move_if_possible(w.value_), value_));
    }

    template<class FunctionLikeValue>
    constexpr auto apply(Writer<LogType, FunctionLikeValue> &&w) &&
    requires _concepts::WithSemigroup<LogType> {
        return _details::make_writer(Semigroup<LogType>().combine(fl::util::move_if_possible(w.log_), fl::util::move_if_possible(log_)),
                                     std::invoke(fl::util::move_if_possible(w.value_), fl::util::move_if_possible(value_)));
    }

    template<class FunctionLikeValue>
    constexpr auto apply(Writer<LogType, FunctionLikeValue> &&w) const &&
    requires _concepts::WithSemigroup<LogType> {
        return _details::make_writer(Semigroup<LogType>().combine(fl::util::move_if_possible(w.log_), fl::util::move_if_possible(log_)),
                                     std::invoke(fl::util::move_if_possible(w.value_), fl::util::move_if_possible(value_)));
    }

    template<class FunctionLikeValue>
    constexpr auto apply(const Writer<LogType, FunctionLikeValue> &w) &&
    requires _concepts::WithSemigroup<LogType> {
        return _details::make_writer(Semigroup<LogType>().combine(w.log_, fl::util::move_if_possible(log_)),
                                     std::invoke(w.value_, fl::util::move_if_possible(value_)));
    }

    template<class FunctionLikeValue>
    constexpr auto apply(const Writer<LogType, FunctionLikeValue> &w) const &&
    requires _concepts::WithSemigroup<LogType> {
        return _details::make_writer(Semigroup<LogType>().combine(w.log_, fl::util::move_if_possible(log_)),
                                     std::invoke(w.value_, fl::util::move_if_possible(value_)));
    }

    auto operator<=> (const Writer&) const = default;
    bool operator== (const Writer&) const = default;

    LogType log_;
    ValueType value_;
};

} // namespace fl
