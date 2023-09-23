//
// MIT License
//
// Copyright (c) 2023-present Vitaly Fanaskov
//
// fl -- Writer "monad" for C++
// Project home: https://github.com/vt4a2h/fl
//
// See LICENSE file for the further details.
//
#include "writer_default_types.hpp"

#include "catch.hpp"

TEST_CASE("Writer copy-move") {
    SECTION("Copy constructor") {
        const Logger logger{{"foo"}, 1};

        const Logger loggerCopy(logger);

        REQUIRE(logger == loggerCopy);
    }

    SECTION("Copy operator") {
        const Logger logger{{"foo"}, 1};

        Logger loggerCopy;
        loggerCopy = logger;

        REQUIRE(logger == loggerCopy);
    }

    SECTION("Move constructor") {
        Logger logger{{"foo"}, 1};

        Logger movedLogger(std::move(logger));

        REQUIRE(movedLogger == Logger{{"foo"}, 1});
    }

    SECTION("Move operator") {
        Logger logger{{"foo"}, 1};

        Logger movedLogger;
        movedLogger = std::move(logger);

        REQUIRE(movedLogger == Logger{{"foo"}, 1});
    }
}
