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

#include <string>

#include <fl/semigroups/semigroup.hpp>

#include <fl/concepts/concepts.hpp>

namespace fl {

namespace _concepts {

template <class T>
concept PossibleToAppend = requires(std::string s, T &&value) {
    { s.append(std::forward<T>(value)) } -> std::same_as<std::add_lvalue_reference_t<std::string>>;
};

} // namespace concepts

template<>
struct Semigroup<std::string> {
    [[nodiscard]]
    std::string combine(concepts::SameOrConstructable<std::string> auto v1, _concepts::PossibleToAppend auto &&v2) const {
        return std::string(v1).append(std::forward<decltype(v2)>(v2));
    }
};
} // namespace fl
