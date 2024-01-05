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

#include <optional>
#include <type_traits>
#include <concepts>

namespace fl {

namespace details {

template<typename>
constexpr bool is_optional = false;

template<typename T>
constexpr bool is_optional<std::optional<T>> = true;

template<class T>
concept IsOptional = is_optional<std::remove_cvref_t<T>>;

template<class T>
concept NotOptional = !is_optional<std::remove_cvref_t<T>>;

template<class V>
constexpr auto pure(V &&v)
{
    if constexpr (IsOptional<V>) {
        return std::forward<V>(v);
    }
    else {
        return std::make_optional(std::forward<V>(v));
    }
}

} // namespace details

template <class F, details::NotOptional ...Args>
constexpr auto ap(F &&f, Args &&...args) noexcept(noexcept(std::invoke(std::forward<F>(f), std::forward<Args>(args)...)))
    requires std::is_invocable_v<F, Args...>
{
    return std::make_optional(std::invoke(std::forward<F>(f), std::forward<Args>(args)...));
}

template <class F, details::IsOptional ...Args>
constexpr auto ap(F &&f, Args &&...args) noexcept(noexcept(ap(std::forward<F>(f), *std::forward<Args>(args)...)))
    -> decltype(ap(std::forward<F>(f), *std::forward<Args>(args)...))
{
    if ((... && args)) {
        return ap(std::forward<F>(f), *std::forward<Args>(args)...);
    } else {
        return {};
    }
}

template <class F, class ...Args>
constexpr auto ap(F &&f, Args &&...args) noexcept(noexcept(ap(std::forward<F>(f), *details::pure(args)...)))
    requires (!std::is_invocable_v<F, Args...> && !(... && details::IsOptional<Args>))
{
    return ap(std::forward<F>(f), details::pure(args)...);
}

} // namespace fl
