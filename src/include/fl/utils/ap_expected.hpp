//
// MIT License
//
// Copyright (c) 2023-present Vitaly Fanaskov
//
// fl -- Writer "monad" for C++
// Project home: https://github.com/vt4a2h/fl
//
// See LICENSE file for the further details.
//
#pragma once

#include <expected>
#include <type_traits>
#include <concepts>

namespace fl {

namespace details {

template<class>
constexpr bool is_expected = false;

template<class T, class V>
constexpr bool is_expected<std::expected<T, V>> = true;

template<class T>
concept IsExpected = is_expected<std::remove_cvref_t<T>>;

template<class T>
concept NotExpected = !is_expected<std::remove_cvref_t<T>>;

template <IsExpected T, IsExpected ...>
struct TakeFirst {
    using error_type = typename std::remove_cvref_t<T>::error_type;
};

template <IsExpected ...T>
using ErrorType = typename TakeFirst<T...>::error_type;

template <IsExpected T, IsExpected ...Ts>
constexpr bool all_same_err = (... && std::is_same_v<ErrorType<T>, ErrorType<Ts>>);

template <class ...Ts>
concept AllSameErr = all_same_err<Ts...>;

template <class Opt, IsExpected E>
constexpr auto operator <<(Opt &&optErr, E &&e) noexcept -> std::remove_cvref_t<Opt>
{
    if (optErr) {
        return optErr;
    } else {
        if (!e) {
            return e.error();
        } else {
            return std::nullopt;
        }
    }
}



template <IsExpected ...Ts>
constexpr std::optional<ErrorType<Ts...>> firstError(Ts &&...ts) noexcept
    requires (sizeof ... (Ts) > 0 && AllSameErr<Ts...>)
{
    return (std::optional<ErrorType<Ts...>>(std::nullopt) << ... << std::forward<Ts>(ts));
}

template<class V>
constexpr auto pure(V &&v)
{
    if constexpr (IsExpected<V>) {
        return std::forward<V>(v);
    }
    else {
        return std::make_optional(std::forward<V>(v)); // Expected!
    }
}

} // namespace details

template <class Error, class F, details::NotExpected ...Args>
constexpr auto ap(F &&f, Args &&...args) noexcept(noexcept(std::invoke(std::forward<F>(f), std::forward<Args>(args)...)))
-> std::expected<std::remove_cvref_t<std::invoke_result_t<F, Args...>>, Error>
    requires std::is_invocable_v<F, Args...>
{
    return std::invoke(std::forward<F>(f), std::forward<Args>(args)...);
}

template <class F, details::IsExpected ...Args>
constexpr auto ap(F &&f, Args &&...args) noexcept(noexcept(std::invoke(std::forward<F>(f), *std::forward<Args>(args)...)))
-> std::expected<decltype(std::invoke(std::forward<F>(f), *std::forward<Args>(args)...)), details::ErrorType<Args...>>
    requires (sizeof ... (Args) > 0)
{
    if (auto e = details::firstError(std::forward<Args>(args)...)) {
        return std::unexpected(*e);
    } else {
        return std::invoke(std::forward<F>(f), *std::forward<Args>(args)...);
    }
}

template <class F, class ...Args>
constexpr auto ap(F &&f, Args &&...args) noexcept(noexcept(ap(std::forward<F>(f), *details::pure(args)...)))
    requires (!std::is_invocable_v<F, Args...> && !(... && details::IsExpected<Args>))
{

}

} // namespace fl
