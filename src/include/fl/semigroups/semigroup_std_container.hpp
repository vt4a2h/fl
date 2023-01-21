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
#include <iostream>

namespace fl {

namespace _concepts {

template <class C>
concept StdContainer = requires(C c) {
    typename C::value_type;
    typename C::size_type;
    typename C::reference;
    typename C::const_reference;
    typename C::iterator;
    typename C::const_iterator;
    typename C::difference_type;

    requires std::default_initializable<C>;
    requires std::move_constructible<C>;
    requires std::copy_constructible<C>;
    requires std::destructible<C>;
    { c.begin() } -> std::same_as<typename C::iterator>;
    { c.end() } -> std::same_as<typename C::iterator>;
    { c.cbegin() } -> std::same_as<typename C::const_iterator>;
    { c.cend() } -> std::same_as<typename C::const_iterator>;
    requires std::equality_comparable<C>;
    requires std::swappable<C>;
    { c.size() } -> std::same_as<typename C::size_type>;
    { c.max_size() } -> std::same_as<typename C::size_type>;
    { c.empty() } -> std::convertible_to<bool>;
};

template<class Container>
concept PushableContainer =
    StdContainer<Container> &&
        requires(Container c, typename Container::value_type &&v) {
    c.reserve(typename Container::size_type{});
    c.push_back(v);
};

template<class Container>
concept InsertableContainer =
StdContainer<Container> &&
    requires(Container c, typename Container::value_type v, typename Container::iterator it) {
        c.insert(v);
        c.insert(it, it);
    };

} // _concepts

template<_concepts::PushableContainer T>
struct Semigroup<T> {
    [[nodiscard]]
    T combine(const T &v1, const T &v2) const {
        fl::_concepts::PushableContainer auto result = v1;
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
        fl::_concepts::PushableContainer auto result = v1;
        result.insert(std::end(result), std::make_move_iterator(std::begin(v2)), std::make_move_iterator(std::end(v2)));
        return result;
    }

    [[nodiscard]]
    T combine(T &&v1, const T& v2) const {
        v1.insert(std::end(v1), std::begin(v2), std::end(v2));
        return std::forward<T>(v1);
    }
};

template<_concepts::InsertableContainer T>
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
        fl::_concepts::InsertableContainer auto result = v1;
        result.insert(std::end(result), std::make_move_iterator(std::begin(v2)), std::make_move_iterator(std::end(v2)));
        return result;
    }

    [[nodiscard]]
    T combine(T &&v1, const T& v2) const {
        fl::_concepts::InsertableContainer auto result = v2;
        result.insert(std::end(result), std::make_move_iterator(std::begin(v1)), std::make_move_iterator(std::end(v1)));
        return result;
    }
};
} // namespace fl
