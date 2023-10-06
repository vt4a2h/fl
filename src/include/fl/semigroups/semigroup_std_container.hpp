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

void append(concepts::PushableContainer auto &r, concepts::PushableContainer auto &&c)
{
    if constexpr (std::is_rvalue_reference_v<decltype(c)>) {
        std::move(std::begin(c), std::end(c), std::back_inserter(r));
    } else {
        std::copy(std::begin(c), std::end(c), std::back_inserter(r));
    }
}

void append(concepts::InsertableContainer auto &r, concepts::InsertableContainer auto &&c)
{
    if constexpr (std::is_rvalue_reference_v<decltype(c)>) {
        r.insert(std::make_move_iterator(std::begin(c)), std::make_move_iterator(std::end(c)));
    } else {
        r.insert(std::begin(c), std::end(c));
    }
}

void reserve(concepts::PushableContainer auto &r, std::size_t size)
{
    r.reserve(size);
}

void reserve(concepts::InsertableContainer auto &, std::size_t) {}

auto combineImpl(concepts::PushableOrInsertableContainer auto &&f, concepts::PushableOrInsertableContainer auto &&s)
    requires all_same<decltype(f), decltype(s)>
{
    if constexpr (std::is_rvalue_reference_v<decltype(f)>) {
        reserve(f, f.size() + s.size());
        append(f, std::forward<decltype(s)>(s));
        return f;
    } else {
        std::remove_cvref_t<decltype(f)> r;
        reserve(r, f.size() + s.size());
        append(r, std::forward<decltype(f)>(f));
        append(r, std::forward<decltype(s)>(s));
        return r;
    }
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
    [[nodiscard]] T combine(concepts::SameContainer<T> auto&& v1, concepts::SameContainer<T> auto&& v2) const {
        return details::combineImpl(std::forward<decltype(v1)>(v1), std::forward<decltype(v2)>(v2));
    }

    [[nodiscard]]
    decltype(auto) combine(concepts::SameContainer<T> auto container, concepts::SameElementType<T> auto &&value) const {
        container.insert(std::forward<decltype(value)>(value));
        return container;
    }
};
} // namespace fl
