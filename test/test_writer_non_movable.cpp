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

#include <string>

#include "fl/writer/writer.hpp"

#include "writer_utility_types.hpp"

TEST_CASE("Writer with non-movable log type") {
    using namespace writer_tests_util;

    using Value = std::uint64_t;
    using Log = NonMovableString;
    using Logger = fl::Writer<Log, Value>;

    SECTION("Tell") {
        Logger logger;

        const auto log = logger.tell(NonMovableString("foo")).tell(NonMovableString("bar")).log();
        REQUIRE(log == NonMovableString("-foo-bar"));
    }

    SECTION("Transform") {
        Logger logger{NonMovableString{"foo"}, 1};

        const auto [l, v] = logger
            .tell(NonMovableString("bar"))
            .transform([](Value val) { return ++val; })
            .as_tuple();

        REQUIRE(l == NonMovableString("foo-bar"));
        REQUIRE(v == Value(2));
    }

    SECTION("And then") {
        const auto [l, v] = Logger{NonMovableString{"foo"}, 1}
            .and_then([](Value val) { return Logger{NonMovableString("bar"), val + 1}; })
            .as_tuple();

        REQUIRE(l == NonMovableString("foo-bar"));
        REQUIRE(v == Value(2));
    }

    SECTION("Apply") {
        using LoggerWithF = fl::Writer<Log, std::function<Value(Value)>>;

        const Logger l{NonMovableString{"foo"}, 10};

        const auto result = l.apply(LoggerWithF{NonMovableString{"bar"}, [](Value v) { return v * 2; }});

        REQUIRE(result.log() == NonMovableString{"bar-foo"});
        REQUIRE(result.value() == 20);
    }
}