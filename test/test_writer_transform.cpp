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

#include "catch.hpp"

TEST_CASE("Writer transform function") {
    SECTION("To the same type") {
        const auto mapped = Logger{{}, 1}.transform([](auto v) { return ++v; });

        REQUIRE(mapped._value == 2);
    }

    SECTION("To the another type") {
        const auto mapped = Logger{{}, 1}.transform([](auto v) { return std::to_string(v); });

        REQUIRE(mapped._value == std::string("1"));
    }

    SECTION("Log is not affected") {
        const Log defaultLogEntry{"1", "2", "3"};
        const auto mapped = Logger{defaultLogEntry, 1}.transform([](auto v) { return ++v; });

        REQUIRE(mapped.log() == defaultLogEntry);
    }
}