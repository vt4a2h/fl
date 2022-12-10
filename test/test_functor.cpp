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
#include "catch.hpp"

#include <fl/experimental/functors/all.hpp>

TEST_CASE("Transform") {
    SECTION("optional with value") {
        auto opt = std::make_optional<int>(42);
        auto f = [](int v) { return std::pow(v, 2); };

        auto functor = fl::Functor<std::optional<int>>();

        auto result = functor.map(opt, f);
        REQUIRE(result.has_value());
        REQUIRE(result.value() == std::pow(42, 2));
    }

    SECTION("optional without value") {
        std::optional<int> opt = std::nullopt;
        auto f = [](int v) { return std::pow(v, 2); };

        auto functor = fl::Functor<std::optional<int>>();

        auto result = functor.map(opt, f);
        REQUIRE_FALSE(result.has_value());
    }
}