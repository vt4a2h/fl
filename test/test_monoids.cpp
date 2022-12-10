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

#include <fl/monoids/all.hpp>
#include <fl/semigroups/all.hpp>

TEST_CASE("Identity") {
    SECTION("Int") {
        auto m = fl::Monoid<int>();

        REQUIRE(m.identity() == int{});
        REQUIRE(m.combine(m.identity(), 1) == 1);
        REQUIRE(m.combine(1, m.identity()) == 1);
    }

    SECTION("String") {
        auto m = fl::Monoid<std::string>();

        REQUIRE(m.identity().empty());
        REQUIRE(m.combine(m.identity(), std::string{"1"}) == std::string{"1"});
        REQUIRE(m.combine(std::string{"1"}, m.identity()) == std::string{"1"});
    }

    SECTION("Vector") {
        using Vec = std::vector<std::string>;
        auto m = fl::Monoid<Vec>();

        REQUIRE(m.identity().empty());
        REQUIRE(m.combine(m.identity(), Vec{"1", "2", "3"}) == Vec{"1", "2", "3"});
        REQUIRE(m.combine(Vec{"1", "2", "3"}, m.identity()) == Vec{"1", "2", "3"});
    }
}