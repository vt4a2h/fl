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

namespace fl
{
    namespace detail
    {
        template <class...>
        struct always_false : std::false_type
        {
        };
    } // namespace detail

    template <class... Args>
    struct Coproduct
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

    template <class T, class P>
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
} // namespace fl
