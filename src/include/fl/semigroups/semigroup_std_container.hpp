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

#include <algorithm>
#include <iterator>

namespace fl {

namespace details {

template <class ...Containers>
inline constexpr bool all_same = sizeof...(Containers) == 0
    || (... && std::is_same_v<std::remove_cvref_t<std::tuple_element_t<0, std::tuple<Containers...>>>,
                              std::remove_cvref_t<Containers>>);

template <class ...Containers>
decltype(auto) combineImpl(Containers &&...containers)
    requires (sizeof...(containers) > 0 && all_same<Containers...>)
{
    std::remove_cvref_t<std::tuple_element_t<0, std::tuple<Containers...>>> r;
    r.reserve((... + containers.size()));
    ([&](auto &&c) {
        if constexpr (std::is_rvalue_reference_v<decltype(c)>) {
            std::move(std::begin(c), std::end(c), std::back_inserter(r));
        } else {
            std::copy(std::begin(c), std::end(c), std::back_inserter(r));
        }
    }(std::forward<Containers>(containers)), ...);

    return r;
}

}

template<concepts::PushableContainer T>
struct Semigroup<T> {
    [[nodiscard]] T combine(concepts::SameContainer<T> auto&& v1, concepts::SameContainer<T> auto&& v2) const {
        return details::combineImpl(std::forward<decltype(v1)>(v1), std::forward<decltype(v2)>(v2));
    }

    [[nodiscard]] T combine(concepts::SameContainer<T> auto container, concepts::SameElementType<T> auto &&value) const {
        container.push_back(std::forward<decltype(value)>(value));
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
