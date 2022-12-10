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

#include <type_traits>

namespace fl::util {

template <class T>
constexpr decltype(auto) move_if_possible(T &&v) {
    if constexpr (std::is_nothrow_move_constructible_v<std::remove_cvref_t<T>>) {
        return std::move(v); // NOLINT
    } else {
        return std::forward<T>(v);
    }
}

} // fl::util