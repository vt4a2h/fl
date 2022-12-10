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

#include <fl/concepts/concepts.hpp>

namespace fl {

template <class T>
struct Functor {
    [[nodiscard]]
    decltype(auto) map(const T &v, concepts::Invocable<T> auto f) { return std::invoke(f, v); }
};

} // namespace fl