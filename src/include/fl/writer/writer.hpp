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

namespace fl {

namespace _concepts {

template <class V>
concept WithMonoid = requires { Monoid<V>(); };

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

    /*!
     * Transform the existing value of writer.
     *
     * @param f function to transform the value.
     * @return a copy of the object with the same log, but transformed value.
     */
    constexpr auto transform(concepts::Invocable<ValueType> auto f) const & {
        return Writer<LogType, std::remove_cvref_t<std::invoke_result_t<decltype(f), ValueType>>>{
            log_, std::invoke(f, value_)
        };
    }

    constexpr auto transform(concepts::Invocable<ValueType> auto f) && {
        return Writer<LogType, std::remove_cvref_t<std::invoke_result_t<decltype(f), ValueType>>>{
            std::move(log_), std::invoke(f, std::move(value_))
        };
    }

    constexpr auto transform(concepts::Invocable<ValueType> auto f) const && {
        return Writer<LogType, std::remove_cvref_t<std::invoke_result_t<decltype(f), ValueType>>>{
            std::move(log_), std::invoke(f, std::move(value_))
        };
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
    constexpr auto tell(concepts::ValidTellEntry<LogType> auto &&l) const & {
        return Writer{Semigroup<LogType>{}.combine(log_, std::forward<decltype(l)>(l)), value_};
    }

    constexpr auto tell(concepts::ValidTellEntry<LogType> auto &&l) && {
        return Writer{Semigroup<LogType>{}.combine(std::move(log_), std::forward<decltype(l)>(l)),
                                     std::move(value_)};
    }

    constexpr auto tell(concepts::ValidTellEntry<LogType> auto &&l) const && {
        return Writer{Semigroup<LogType>{}.combine(std::move(log_), std::forward<decltype(l)>(l)),
                                     std::move(value_)};
    }

    constexpr auto tell(concepts::CombinableWithSgWrapper<LogType> auto &&l, const SemigroupWrapper<LogType> &sg) const & {
        return Writer{sg.combine(log_, std::forward<decltype(l)>(l)), value_};
    }

    constexpr auto tell(concepts::CombinableWithSgWrapper<LogType> auto &&l, const SemigroupWrapper<LogType> &sg) && {
        return Writer{sg.combine(std::move(log_), std::forward<decltype(l)>(l)), std::move(value_)};
    }

    constexpr auto tell(concepts::CombinableWithSgWrapper<LogType> auto &&l, const SemigroupWrapper<LogType> &sg) const && {
        return Writer{sg.combine(std::move(log_), std::forward<decltype(l)>(l)), std::move(value_)};
    }

    /*!
     * Change places of value and log.
     *
     * @return a new writer object of the type Writer<Value, Log>.
     */
    constexpr auto swap() const & {
        return Writer<ValueType, LogType>{value_, log_};
    }

    constexpr auto swap() && {
        return Writer<ValueType, LogType>{std::move(value_), std::move(log_)};
    }

    constexpr auto swap() const && {
        return Writer<ValueType, LogType>{std::move(value_), std::move(log_)};
    }

    /*!
     * Get value.
     * @return a copy of the value object.
     */
    constexpr auto value() const & {
        return value_;
    }

    constexpr auto value() && {
        return std::move(value_);
    }

    constexpr auto value() const && {
        return std::move(value_);
    }

    /*!
     * Get log.
     * @return a copy of the log object.
     */
    constexpr auto log() const & {
        return log_;
    }

    constexpr auto log() && {
        return std::move(log_);
    }

    constexpr auto log() const && {
        return std::move(log_);
    }

    /*!
     * Tuple of log and value objects.
     * @return a tuple of copies of log and value objects.
     */
    constexpr auto as_tuple() const & {
        return std::make_tuple(log_, value_);
    }

    constexpr auto as_tuple() && {
        return std::make_tuple(std::move(log_), std::move(value_));
    }

    constexpr auto as_tuple() const && {
        return std::make_tuple(std::move(log_), std::move(value_));
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
        return Writer{Monoid<LogType>().identity(), value_};
    }

    /*!
     * Compose this writer with the function \p f.
     * @param f function that accepts Writer object of this type. The function must also return a writer.
     * @return a new writer object.
     */
    constexpr auto and_then(concepts::InvocableAndReturnsWriter<ValueType> auto f) const &
    requires concepts::WithSemigroup<LogType> {
        auto &&w = std::invoke(f, value_);
        return std::remove_cvref_t<decltype(w)>{
            Semigroup<LogType>().combine(log_, std::move(w.log_)),
            std::move(w.value_)
        };
    }

    constexpr auto and_then(concepts::InvocableAndReturnsWriter<ValueType> auto f) &&
    requires concepts::WithSemigroup<LogType> {
        auto &&w = std::invoke(f, std::move(value_));
        return std::remove_cvref_t<decltype(w)>{
            Semigroup<LogType>().combine(std::move(log_), std::move(w.log_)),
            std::move(w.value_)
        };
    }

    constexpr auto and_then(concepts::InvocableAndReturnsWriter<ValueType> auto f) const &&
    requires concepts::WithSemigroup<LogType> {
        auto &&w = std::invoke(f, std::move(value_));
        return std::remove_cvref_t<decltype(w)>{
            Semigroup<LogType>().combine(std::move(log_), std::move(w.log_)),
            std::move(w.value_)
        };
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
    requires concepts::WithSemigroup<LogType> {
        return Writer{Semigroup<LogType>().combine(w.log_, log_), std::invoke(w.value_, value_)};
    }

    template<class FunctionLikeValue>
    constexpr auto apply(Writer<LogType, FunctionLikeValue> &&w) const &
    requires concepts::WithSemigroup<LogType> {
        return Writer{Semigroup<LogType>().combine(std::move(w.log_), log_),
                                     std::invoke(std::move(w.value_), value_)};
    }

    template<class FunctionLikeValue>
    constexpr auto apply(Writer<LogType, FunctionLikeValue> &&w) &&
    requires concepts::WithSemigroup<LogType> {
        return Writer{Semigroup<LogType>().combine(std::move(w.log_), std::move(log_)),
                                     std::invoke(std::move(w.value_), std::move(value_))};
    }

    template<class FunctionLikeValue>
    constexpr auto apply(Writer<LogType, FunctionLikeValue> &&w) const &&
    requires concepts::WithSemigroup<LogType> {
        return Writer{Semigroup<LogType>().combine(std::move(w.log_), std::move(log_)),
                                     std::invoke(std::move(w.value_), std::move(value_))};
    }

    template<class FunctionLikeValue>
    constexpr auto apply(const Writer<LogType, FunctionLikeValue> &w) &&
    requires concepts::WithSemigroup<LogType> {
        return Writer{Semigroup<LogType>().combine(w.log_, std::move(log_)),
                                     std::invoke(w.value_, std::move(value_))};
    }

    template<class FunctionLikeValue>
    constexpr auto apply(const Writer<LogType, FunctionLikeValue> &w) const &&
    requires concepts::WithSemigroup<LogType> {
        return Writer{Semigroup<LogType>().combine(w.log_, std::move(log_)),
                                     std::invoke(w.value_, std::move(value_))};
    }

    auto operator<=> (const Writer&) const = default;
    bool operator== (const Writer&) const = default;

    LogType log_;
    ValueType value_;
};

} // namespace fl
