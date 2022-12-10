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

TEST_CASE("Writer flat transform function") {
    SECTION("Only value can be changed") {
        const auto l = Logger{{"foo"}, 1}.and_then([](auto v) { return Logger{{}, ++v}; });

        REQUIRE(l.log() == Log{"foo"});
        REQUIRE(l.value() == 2);
    }

    SECTION("Only log can be changed") {
        const auto l = Logger{{"foo"}, 1}.and_then([](auto v) { return Logger{{"bar"}, v}; });

        REQUIRE(l.log() == Log{"foo", "bar"});
        REQUIRE(l.value() == 1);
    }

    SECTION("Log and value can be changed") {
        const auto l = Logger{{"foo"}, 1}.and_then([](auto v) { return Logger{{"bar"}, ++v}; });

        REQUIRE(l.log() == Log{"foo", "bar"});
        REQUIRE(l.value() == 2);
    }

    SECTION("This is possible to change a type of value") {
        using AnotherLogger = fl::Writer<Log, double>;

        const auto l = Logger{{"foo"}, 1}.and_then([](auto) { return AnotherLogger{{"bar"}, 42.2}; });
        static_assert(std::is_same_v<std::remove_cvref_t<decltype(l)>, AnotherLogger>);

        REQUIRE(l.log() == Log{"foo", "bar"});
        REQUIRE(l.value() == double(42.2));
    }
}