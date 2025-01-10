//
// MIT License
//
// Copyright (c) 2025-present Vitaly Fanaskov
//
// fl -- Functional tools for C++
// Project home: https://github.com/vt4a2h/fl
//
// See LICENSE file for the further details.
//
#pragma once

#include <variant>
#include <concepts>
#include <type_traits>

namespace fl {

namespace details {

template <class>
constexpr bool is_variant = false;

template <class ...Args>
constexpr bool is_variant<std::variant<Args...>> = true;

template <class Variant>
concept IsVariant = is_variant<std::remove_cvref_t<Variant>>;

template <class F, IsVariant Variant>
constexpr bool invocable_with()
{
    return []<std::size_t... I>(std::index_sequence<I...>){
        return (... || std::is_invocable_v<F, std::variant_alternative_t<I, Variant>>);
    }(std::make_index_sequence<std::variant_size_v<Variant>>{});
}

template <class F, class Variant>
concept InvocableWithAnyOf = invocable_with<std::remove_cvref_t<F>, std::remove_cvref_t<Variant>>();

template <std::size_t Index, class Variant, class Value, class Callable>
bool invokeIfCan(Value &valuePtr, Callable f)
{
    if constexpr (std::is_invocable_v<Callable, std::variant_alternative_t<Index, Variant>>) {
        std::invoke(f, valuePtr);
        return true;
    } else {
        return false;
    }
}

template <std::size_t Index, class Variant_t, details::IsVariant Variant, details::InvocableWithAnyOf<Variant> ...Callable>
bool invokeIfHoldsCorrectAlternative(const Variant &variant, const Callable &...callable)
{
    auto *v = std::get_if<std::variant_alternative_t<Index, Variant_t>>(&variant);
    return v && ((... || invokeIfCan<Index, Variant_t>(*v, callable)));
}

} // namespace details

template <details::IsVariant Variant, details::InvocableWithAnyOf<Variant> ...Callable>
void match(const Variant &variant, Callable ...callable)
{
    using Variant_t = std::remove_cvref_t<Variant>;

    [&variant, &callable...]<std::size_t... I>(std::index_sequence<I...>){
        (... || details::invokeIfHoldsCorrectAlternative<I, Variant_t>(variant, callable...));
    }(std::make_index_sequence<std::variant_size_v<Variant_t>>{});
}

} // namespace fl