//
// MIT License
//
// Copyright (c) 2024-present Vitaly Fanaskov
//
// fl -- Functional tools for C++
// Project home: https://github.com/vt4a2h/fl
//
// See LICENSE file for the further details.
//
#include "catch.hpp"

#include <fl/expected/expected.hpp>

constexpr fl::Expected<int, std::string> expectedlyAdd(int v)
{
    fl::Expected<int, std::string> expected{1};
    expected.value() += v;

    return expected;
}

TEST_CASE("Access expected value")
{
    SECTION("By ref")
    {
        fl::Expected<int, std::string> expected(41);

        ++expected.value();

        REQUIRE(expected.value() == 42);
    }

    SECTION("[constexpr] By ref")
    {
        static constexpr auto expected = expectedlyAdd(41);

        STATIC_REQUIRE(expected.value() == 42);
    }

    SECTION("By const ref")
    {
        const fl::Expected<int, std::string> expected(42);

        REQUIRE(expected.value() == 42);
    }

    SECTION("[constexpr] By const ref")
    {
        static constexpr fl::Expected<int, std::string> expected(42);
        static constexpr auto &error = expected.value();

        STATIC_REQUIRE(error == 42);
    }
}