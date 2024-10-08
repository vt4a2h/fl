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

#include <string>

TEST_CASE("Create unexpected")
{
    SECTION("From value")
    {
        std::string data{"42"};
        const auto unexpected = fl::Unexpected(data);

        REQUIRE(unexpected.error() == data);
    }

    SECTION("[Constexpr] From value")
    {
        static constexpr std::string data{"42"};
        static constexpr auto unexpected = fl::Unexpected(data);

        STATIC_REQUIRE(unexpected.error() == data);
    }

    SECTION("From value in-place")
    {
        const auto unexpected = fl::Unexpected<std::string>(std::in_place_t{}, "42");

        REQUIRE(unexpected.error() == "42");
    }

    SECTION("[Constexpr] From value in-place")
    {
        static constexpr auto unexpected = fl::Unexpected<std::string>(std::in_place_t{}, "42");

        STATIC_REQUIRE(unexpected.error() == "42");
    }

    SECTION("From value in-place with initializer list")
    {
        const auto unexpected = fl::Unexpected<std::string>(std::in_place_t{}, {'4', '2'}, std::allocator<char>{});

        REQUIRE(unexpected.error() == std::string{"42"});
    }

    SECTION("[Constexpr] From value in-place with initializer list")
    {
        static constexpr auto unexpected = fl::Unexpected<std::string>(std::in_place_t{}, {'4', '2'}, std::allocator<char>{});

        REQUIRE(unexpected.error() == std::string{"42"});
    }
}
