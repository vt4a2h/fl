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

#include "functor.hpp"

#include <optional>

namespace fl {

namespace _concepts {

template<class F, class T>
concept InvocableAsOptional = requires {
    requires concepts::Invocable<F, T>;
    std::is_destructible_v<std::invoke_result_t<F, T>>;
    std::is_constructible_v<std::optional<std::invoke_result_t<F, T>>, std::invoke_result_t<F, T>>;
};

} // namespace _concepts

template<class T>
struct Functor<std::optional<T>> {
    [[nodiscard]]
    decltype(auto) map(const std::optional<T> &v, _concepts::InvocableAsOptional<T> auto f) {
        // Due to dull MSVC limitation
        using ReturnType = std::optional<std::invoke_result_t<decltype(f), T>>;

        if (v.has_value())
            return ReturnType(std::invoke(f, v.value()));

        return ReturnType(std::nullopt);
    }
};

} // namespace fl