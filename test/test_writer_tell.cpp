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

TEST_CASE("Writer method tell") {
    SECTION("Add a single entity for empty logger") {
        REQUIRE(Logger{}.tell({"foo"}).log() == Log{"foo"});
    }

    SECTION("Combine several entries") {
        REQUIRE(Logger{}.tell({"foo"}).tell({"bar"}).log() == Log{"foo", "bar"});
    }

    SECTION("Not change value") {
        REQUIRE(Logger{{}, 1}.tell({"foo"}).tell({"bar"}).value() == 1);
    }

    SECTION("From regular value") {
        const auto l = Logger{};
        REQUIRE(l.tell({"foo"}).log() == Log{"foo"});
    }

    SECTION("From moved value") {
        const auto l = Logger{};

        Log foo{
            {"Lorem ipsum dolor sit amet, consectetur adipiscing elit. Etiam fermentum erat nec hendrerit interdum."}};

        auto l2 = l.tell(std::move(foo));

        REQUIRE(l2.log() == Log{
            {"Lorem ipsum dolor sit amet, consectetur adipiscing elit. Etiam fermentum erat nec hendrerit interdum."}});
    }

    SECTION("Combine loger with an element") {
        const auto l = Logger{};
        std::string logEntry{"foo"};

        const auto result = l.tell(logEntry).tell(logEntry);

        REQUIRE(result.log() == std::vector<std::string>{"foo", "foo"});
    }

    SECTION("Combine loger with convertible element") {
        const auto result = Logger{}.tell("foo").tell("foo");

        REQUIRE(result.log() == std::vector<std::string>{"foo", "foo"});
    }
}