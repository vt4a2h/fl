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
#include "writer_default_types.hpp"
#include <fl/monoids/all.hpp>

#include "catch.hpp"

TEST_CASE("Writer other methods") {
    SECTION("bindings") {
        const auto w = Logger{{}, 1};
        const auto &[l, v] = w;

        REQUIRE(l.empty());
        REQUIRE(v == 1);
    }

    SECTION("swap &&") {
        auto logger = Logger{{}, 1}.swap();

        REQUIRE(logger.log() == 1);
        REQUIRE(logger.value().empty());
    }

    SECTION("swap &") {
        const auto logger = Logger{{}, 1};
        const auto logger2 = logger.swap();

        REQUIRE(logger2.log() == 1);
        REQUIRE(logger2.value().empty());
    }

    SECTION("value") {
        REQUIRE(Logger{{}, 1}.value() == 1);
    }

    SECTION("log") {
        REQUIRE(Logger{{}, 1}.log().empty());
    }

    SECTION("as_tuple &&") {
        REQUIRE(Logger{{}, 1}.as_tuple() == std::make_tuple(Log{}, Value{1}));
    }

    SECTION("as_tuple &") {
        const auto l = Logger{{}, 1};
        REQUIRE(l.as_tuple() == std::make_tuple(Log{}, Value{1}));
    }

    SECTION("reset") {
        const auto logger = Logger{Log{}, 1}.tell("foo");
        REQUIRE(logger.reset().log().empty());
    }
}