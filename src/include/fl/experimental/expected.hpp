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

#include <fl/coproduct/coproduct.hpp>

namespace fl::experimental
{
    template <class T>
    struct Ok
    {
        T value;
    };

    template <class T>
    struct Err
    {
        T value;
    };

    template <class O, class E>
    struct Expected : Coproduct<Ok<O>, Err<E>>
    {
        using Coproduct<Ok<O>, Err<E>>::Coproduct;
    };
} // fl::experimental
