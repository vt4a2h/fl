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
#include <limits>

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
    }

    template <class... Args>
    struct Coproduct;

    namespace detail
    {
        template <class...>
        struct always_false : std::false_type
        {
        };
    } // namespace detail

    template <class MaybeCoproduct>
    concept IsCoproduct = std::is_base_of_v<detail::coproduct_base_tag, std::remove_cvref_t<MaybeCoproduct>>;

    template <class... Args>
    struct Coproduct : detail::coproduct_base_tag
    {
        static_assert(detail::always_false<Args...>::value, "Coproduct is not implemented for this number of types");
    };

    template <class T>
    struct Coproduct<T>
    {
        using value_t = std::remove_cvref_t<T>;
        T value;
    };

    template <class T0, class T1>
    struct Coproduct<T0, T1>
    {
        using value_t_0 = std::remove_cvref_t<T0>;
        using value_t_1 = std::remove_cvref_t<T1>;

        union Data
        {
            value_t_0 v0;
            value_t_1 v1;

            ~Data()
            {
                /* clang and gcc can implicitly remove a destructor in case when one of the types is non-trivial */
            }
        } value;

        bool is_v0;
    };

    template <class>
    constexpr std::size_t arity = std::numeric_limits<std::size_t>::min();

    template <class... Args>
    constexpr std::size_t arity<Coproduct<Args...>> = sizeof...(Args);

    template <class, class P>
    constexpr bool holds_value_of_type(const P&)
    {
        return false;
    }

    template <class T, class P>
        requires (arity<std::remove_cvref_t<P>> == 1)
    constexpr bool holds_value_of_type(const P&)
    {
        return std::is_same_v<typename std::remove_cv_t<P>::value_t, T>;
    }

    template <class T, class P>
        requires (arity<std::remove_cvref_t<P>> == 2)
    constexpr bool holds_value_of_type(const P& p)
    {
        return p.is_v0
                   ? std::is_same_v<typename std::remove_cv_t<P>::value_t_0, T>
                   : std::is_same_v<typename std::remove_cv_t<P>::value_t_1, T>;
    }

    template <IsCoproduct P, class... Matchers>
    decltype(auto) match(P&&, Matchers&&...)
    {
        static_assert(detail::always_false<P, Matchers...>::value,
                      "Match is not implemented for this number of types");
    }

    template <IsCoproduct P, class Matcher>
        requires (
            arity<std::remove_cvref_t<P>> == 1 &&
            std::is_invocable_v<Matcher, typename std::remove_cvref_t<P>::value_t>
        )
    decltype(auto) match(P&& p, Matcher&& m)
    {
        return std::invoke(std::forward<Matcher>(m), std::forward<P>(p.val));
    }

    template <IsCoproduct P, class... Matchers>
        requires (
            arity<std::remove_cvref_t<P>> == 2 &&
            sizeof...(Matchers) == 2 &&
            (... || (
                std::is_invocable_v<Matchers, typename std::remove_cvref_t<P>::value_t_0> ||
                std::is_invocable_v<Matchers, typename std::remove_cvref_t<P>::value_t_1>))
        )
    decltype(auto) match(P&& p, Matchers&&... m)
    {
        const auto matcher = detail::overloads{std::forward<Matchers>(m)...};

        if (holds_value_of_type<typename std::remove_cvref_t<P>::value_t_0>(p))
        {
            if (std::is_invocable_v<decltype(matcher), typename std::remove_cvref_t<P>::value_t_0>)
            {
                return std::invoke(matcher, std::forward<P>(p.val).v0);
            }
        }
        else
        {
            if (std::is_invocable_v<decltype(matcher), typename std::remove_cvref_t<P>::value_t_1>)
            {
                return std::invoke(matcher, std::forward<P>(p.val).v1);
            }
        }
    }
} // namespace fl
