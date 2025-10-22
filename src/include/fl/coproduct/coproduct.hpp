//
// MIT License
//
// Copyright (c) 2024-present Vitaly Fanaskov
//
// fl -- Functional tools for C++
// Project home: https://github.com/vt4a2h/fl
//
// See LICENSE file for the further details.
//
#pragma once

#include <type_traits>
#include <variant>

namespace fl
{
    namespace detail
    {
        struct coproduct_base_tag
        {
        };

        template <class... Ts>
        struct overloads : Ts...
        {
            using Ts::operator()...;
        };
    } // namespace detail

    template <class... Args>
    struct Coproduct : std::variant<Args...>, detail::coproduct_base_tag
    {
        using std::variant<Args...>::variant;
    };

    template <class MaybeCoproduct>
    concept IsCoproduct = std::is_base_of_v<detail::coproduct_base_tag, std::remove_cvref_t<MaybeCoproduct>>;

    template <class>
    constexpr std::size_t arity = std::numeric_limits<std::size_t>::min();

    template <class... Args>
    constexpr std::size_t arity<Coproduct<Args...>> = sizeof...(Args);

    template <class T, IsCoproduct C>
    constexpr bool holds_value_of_type(const C& coproduct)
    {
        return std::holds_alternative<T>(coproduct);
    }

    template <class T, IsCoproduct C>
    constexpr decltype(auto) get_value(C&& coproduct) noexcept
    {
        assert(holds_value_of_type<T>(coproduct)); // TODO: should be a pre-condition
        return std::get<T>(std::forward<C>(coproduct));
    }

    template <IsCoproduct C, class... Matchers>
    decltype(auto) match(C&& coproduct, Matchers&&... matchers)
    {
        return std::visit(detail::overloads{std::forward<Matchers>(matchers)...}, std::forward<C>(coproduct));
    }
} // namespace fl
