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

#include <tuple>

namespace fl {

template<class T>
struct callable_traits;

template<class T>
struct callable_traits : public callable_traits<decltype(&T::operator())> {};

template<class R, class ...Args>
struct callable_traits<R (*)(Args...)>
{
    static constexpr std::size_t args_count = sizeof...(Args);

    using result_type = R;

    template <std::size_t index>
        requires (index < args_count)
    struct arg
    {
        using type = typename std::tuple_element<index, std::tuple<Args...>>::type;
    };
};

template<class R, class C, class ...Args>
struct callable_traits<R (C::*)(Args...) const> : public callable_traits<R (*)(Args...)>
{
    using class_type = C;
};

} // namespace fl