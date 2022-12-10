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

TEST_CASE("Writer create") {
    SECTION("Default logger is empty") {
        const Logger logger{};

        REQUIRE(logger.value() == Value{});
        REQUIRE(logger.log() == Log{});
    }

    SECTION("Can be initialized with default log") {
        const Log defaultLogEntry{"1", "2", "3"};
        const Logger logger{defaultLogEntry, {}};

        REQUIRE(logger.log() == defaultLogEntry);
    }

    SECTION("Can be initialized with default value") {
        const Value defaultValue{42};
        const Logger logger{{}, defaultValue};

        REQUIRE(logger.value() == defaultValue);
    }

    SECTION("Can be initialized with default log and value") {
        const Value defaultValue{42};
        const Log defaultLogEntry{"1", "2", "3"};

        const Logger logger{defaultLogEntry, defaultValue};

        REQUIRE(logger.log() == defaultLogEntry);
        REQUIRE(logger.value() == defaultValue);
    }
}
