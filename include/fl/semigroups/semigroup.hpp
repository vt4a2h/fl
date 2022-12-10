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

namespace fl {
template<class T>
struct Semigroup {
    [[nodiscard]]
    T combine(const T& v1, const T& v2) const;

    [[nodiscard]]
    T combine(T&& v1, T&& v2) const;

    [[nodiscard]]
    T combine(const T& v1, T&& v2) const;

    [[nodiscard]]
    T combine(T&& v1, const T& v2) const;
};
} // namespace fl