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

TEST_CASE("Check expected on compile time")
{
    SECTION("Is expected")
    {
        STATIC_REQUIRE(fl::concepts::IsExpected<fl::Expected<int, std::string>>);
        STATIC_REQUIRE(fl::concepts::IsExpected<const fl::Expected<int, std::string>>);
        STATIC_REQUIRE(fl::concepts::IsExpected<const fl::Expected<int, std::string>&>);
        STATIC_REQUIRE(fl::concepts::IsExpected<fl::Expected<int, std::string>&>);
        STATIC_REQUIRE(fl::concepts::IsExpected<fl::Expected<int, std::string>&&>);
    }

    SECTION("Is not expected")
    {
        STATIC_REQUIRE_FALSE(fl::concepts::IsExpected<std::string>);
        STATIC_REQUIRE_FALSE(fl::concepts::IsExpected<const std::string>);
        STATIC_REQUIRE_FALSE(fl::concepts::IsExpected<const std::string&>);
        STATIC_REQUIRE_FALSE(fl::concepts::IsExpected<std::string&>);
        STATIC_REQUIRE_FALSE(fl::concepts::IsExpected<std::string&&>);
    }

    SECTION("Is unexpected")
    {
        STATIC_REQUIRE(fl::concepts::IsUnexpected<fl::Unexpected<int>>);
        STATIC_REQUIRE(fl::concepts::IsUnexpected<const fl::Unexpected<int>>);
        STATIC_REQUIRE(fl::concepts::IsUnexpected<const fl::Unexpected<int>&>);
        STATIC_REQUIRE(fl::concepts::IsUnexpected<fl::Unexpected<int>&>);
        STATIC_REQUIRE(fl::concepts::IsUnexpected<fl::Unexpected<int>&&>);
    }

    SECTION("Is unexpected")
    {
        STATIC_REQUIRE_FALSE(fl::concepts::IsUnexpected<std::string>);
        STATIC_REQUIRE_FALSE(fl::concepts::IsUnexpected<const std::string>);
        STATIC_REQUIRE_FALSE(fl::concepts::IsUnexpected<const std::string&>);
        STATIC_REQUIRE_FALSE(fl::concepts::IsUnexpected<std::string&>);
        STATIC_REQUIRE_FALSE(fl::concepts::IsUnexpected<std::string&&>);
    }
}