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

constexpr fl::Expected<std::string, int> unexpectedlyAdd(int v)
{
    fl::Expected<std::string, int> expected{fl::Unexpected{1}};
    expected.error() += v;

    return expected;
}

TEST_CASE("Access expected error")
{
    SECTION("By ref")
    {
        fl::Expected<std::string, int> expected(fl::Unexpected{41});

        ++expected.error();

        REQUIRE(expected.error() == 42);
    }

    SECTION("[constexpr] By ref")
    {
        static constexpr auto expected = unexpectedlyAdd(41);

        STATIC_REQUIRE(expected.error() == 42);
    }

    SECTION("By const ref")
    {
        const fl::Expected<std::string, int> expected(fl::Unexpected{42});
        const auto &error = expected.error();

        REQUIRE(error == 42);
    }

    SECTION("[constexpr] By const ref")
    {
        static constexpr fl::Expected<std::string, int> expected(fl::Unexpected{42});;
        static constexpr auto &error = expected.error();

        STATIC_REQUIRE(error == 42);
    }
}
