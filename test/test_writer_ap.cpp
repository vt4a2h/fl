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

TEST_CASE("Writer apply") {
    using LoggerWithF = fl::Writer<Log, std::function<Value(Value)>>;

    SECTION("Const writers") {
        const Logger l{{"foo"}, 10};
        const LoggerWithF appL{{"bar"}, [](Value v) { return v * 2; }};

        const auto result = l.apply(appL);

        REQUIRE(result.log() == Log{"bar", "foo"});
        REQUIRE(result.value() == 20);
    }

    SECTION("Const writer tmp apply") {
        const Logger l{{"foo"}, 10};

        const auto result = l.apply(LoggerWithF{{"bar"}, [](Value v)
        { return v * 2; }});

        REQUIRE(result.log() == Log{"bar", "foo"});
        REQUIRE(result.value() == 20);
    }

    SECTION("Tmp writer const apply") {
        const LoggerWithF appL{{"bar"}, [](Value v) { return v * 2; }};

        const auto result = Logger{{"foo"}, 10}.apply(appL);

        REQUIRE(result.log() == Log{"bar", "foo"});
        REQUIRE(result.value() == 20);
    }

    SECTION("Tmp writer tmp apply") {
        const auto result = Logger{{"foo"}, 10}.apply(LoggerWithF{{"bar"}, [](Value v)
        { return v * 2; }});

        REQUIRE(result.log() == Log{"bar", "foo"});
        REQUIRE(result.value() == 20);
    }
}
