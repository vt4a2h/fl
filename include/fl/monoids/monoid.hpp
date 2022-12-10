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

#include <fl/semigroups/semigroup.hpp>

namespace fl {

template<class T>
struct Monoid : public Semigroup<T> {
    [[nodiscard]]
    T identity() const;
};

} // namespace fl
