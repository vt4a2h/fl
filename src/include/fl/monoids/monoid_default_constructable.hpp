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

#include <fl/monoids/monoid.hpp>

#include <type_traits>

namespace fl {

namespace _concepts {
template<class T>
concept DefaultConstructable = requires {
    std::is_default_constructible_v<T>;
};
} // namespace _concepts

template<_concepts::DefaultConstructable T>
struct Monoid<T> : public Semigroup<T> {
    [[nodiscard]]
    T identity() const {
        return T{};
    }
};

} // namespace fl
