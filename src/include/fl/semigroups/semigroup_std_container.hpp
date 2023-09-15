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
#include <fl/concepts/concepts.hpp>

namespace fl {

template<concepts::PushableContainer T>
struct Semigroup<T> {
    [[nodiscard]]
    T combine(const T &v1, const T &v2) const {
        fl::concepts::PushableContainer auto result = v1;
        result.insert(std::end(result), std::begin(v2), std::end(v2));
        return result;
    }

    [[nodiscard]]
    T combine(T&& v1, T&& v2) const {
        v1.insert(std::end(v1), std::make_move_iterator(std::begin(v2)), std::make_move_iterator(std::end(v2)));
        return std::forward<T>(v1);
    }

    [[nodiscard]]
    T combine(const T &v1, T&& v2) const {
        fl::concepts::PushableContainer auto result = v1;
        result.insert(std::end(result), std::make_move_iterator(std::begin(v2)), std::make_move_iterator(std::end(v2)));
        return result;
    }

    [[nodiscard]]
    T combine(T &&v1, const T& v2) const {
        v1.insert(std::end(v1), std::begin(v2), std::end(v2));
        return std::forward<T>(v1);
    }

    template<concepts::SameContainer<T> Container, concepts::SameElementType<Container> Value>
    [[nodiscard]] decltype(auto) combine(Container container, Value &&value) const {
        container.emplace_back(std::forward<Value>(value));
        return container;
    }
};

template<concepts::InsertableContainer T>
struct Semigroup<T> {
    [[nodiscard]]
    T combine(const T& v1, const T& v2) const {
        T c;

        c.insert(std::begin(v1), std::end(v1));
        c.insert(std::begin(v2), std::end(v2));

        return c;
    }

    [[nodiscard]]
    T combine(T&& v1, T&& v2) const {
        v1.insert(std::make_move_iterator(std::begin(v2)), std::make_move_iterator(std::end(v2)));
        return v1;
    }

    [[nodiscard]]
    T combine(const T &v1, T&& v2) const {
        fl::concepts::InsertableContainer auto result = v1;
        result.insert(std::end(result), std::make_move_iterator(std::begin(v2)), std::make_move_iterator(std::end(v2)));
        return result;
    }

    [[nodiscard]]
    T combine(T &&v1, const T& v2) const {
        fl::concepts::InsertableContainer auto result = v2;
        result.insert(std::end(result), std::make_move_iterator(std::begin(v1)), std::make_move_iterator(std::end(v1)));
        return result;
    }

    template<concepts::SameContainer<T> Container, concepts::SameElementType<Container> Value>
    [[nodiscard]] decltype(auto) combine(Container container, Value &&value) const {
        container.insert(std::forward<Value>(value));
        return container;
    }
};
} // namespace fl
