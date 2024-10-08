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

TEST_CASE("Access unexpected error")
{
    SECTION("By ref")
    {
        auto unexpected = fl::Unexpected(42);

        ++unexpected.error();

        REQUIRE(unexpected.error() == 43);
    }

    SECTION("[constexpr] By const ref")
    {
        static constexpr auto unexpected = fl::Unexpected(42);
        static constexpr auto &error = unexpected.error();

        STATIC_REQUIRE(error == 42);
    }
}