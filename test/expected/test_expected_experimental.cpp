//
// MIT License
//
// Copyright (c) 2025-present Vitaly Fanaskov
//
// fl -- Functional tools for C++
// Project home: https://github.com/vt4a2h/fl
//
// See LICENSE file for the further details.
//
#include "catch.hpp"

#include <fl/expected/expected.hpp>

using namespace fl::experimental;

TEST_CASE("Create")
{
    SECTION("Has default constructed value by default")
    {
        expected<double, std::string> e;
        REQUIRE(e);
    }

    SECTION("Has value when set")
    {
        const expected<double, std::string> e{42.};
        REQUIRE(e.has_value());
    }

    SECTION("Has error when set")
    {
        const expected<double, std::string> e{"error"};
        REQUIRE(e.has_error());
    }
}

TEST_CASE("Comparable")
{
    using Expected = expected<int, std::string>;
    const auto [lhs, rhs, equal] = GENERATE(table<Expected, Expected, bool>({
        {Expected{42}, Expected{42}, true},
        {Expected{}, Expected{}, true},
        {Expected{"42"}, Expected{"42"}, true},
        {Expected{42}, Expected{"42"}, false},
        {Expected{"42"}, Expected{42}, false},
    }));

    REQUIRE((lhs == rhs) == equal);
}