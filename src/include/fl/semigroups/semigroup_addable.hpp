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
#include <type_traits>

#include <fl/semigroups/semigroup.hpp>

namespace fl {

namespace _concepts {
template<class T>
concept Addable = requires(T v1, T v2) {
    { v1 + v2 } -> std::same_as<T>;
};
} // namespace _concepts

template<_concepts::Addable T>
struct Semigroup<T> {
    [[nodiscard]]
    T combine(const T& v1, const T& v2) const {
        return v1 + v2;
    }

    [[nodiscard]]
    T combine(T&& v1, T&& v2) const {
        return v1 + v2;
    }
};
} // namespace fl
