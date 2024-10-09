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

namespace test::details {

inline static constexpr int default_value{42};

struct Default {
    int data{default_value};
};

} // namespace test::details

TEST_CASE("Expected create")
{
    SECTION("Default constructable") {
        const fl::Expected<int, std::string> expectedBox;

        REQUIRE(expectedBox.hasValue());
    }

    SECTION("Default constructable with correct value") {
        const fl::Expected<test::details::Default, std::string> expectedBox;

        REQUIRE(expectedBox.value().data == test::details::default_value);
    }

    SECTION("[Constexpr] Default constructable") {
        constexpr fl::Expected<int, std::string> expectedBox;

        STATIC_REQUIRE(expectedBox.hasValue());
    }

    SECTION("[Constexpr] Default constructable with correct value") {
        constexpr fl::Expected<test::details::Default, std::string> expectedBox;

        STATIC_REQUIRE(expectedBox.value().data == test::details::default_value);
    }

    SECTION("From unexpected")
    {
        const fl::Expected<int, std::string> expectedBox(fl::Unexpected{std::string{}});

        REQUIRE(expectedBox.hasError());
    }

    SECTION("From unexpected with correct value")
    {
        const std::string unexpectedValue{"42"};
        const fl::Expected<int, std::string> expectedBox(fl::Unexpected{unexpectedValue});

        REQUIRE(expectedBox.error() == unexpectedValue);
    }

    SECTION("[Constexpr] From unexpected")
    {
        static constexpr std::string string;
        static constexpr auto unexpected = fl::Unexpected(string);
        static constexpr fl::Expected<int, std::string> expectedBox(unexpected);

        STATIC_REQUIRE(expectedBox.hasError());
    }

    SECTION("[Constexpr] From unexpected with correct value")
    {
        static constexpr std::string string{"42"};
        static constexpr auto unexpected = fl::Unexpected(string);
        static constexpr fl::Expected<int, std::string> expectedBox(unexpected);

        STATIC_REQUIRE(expectedBox.error() == string);
    }
}