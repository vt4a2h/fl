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

constexpr fl::Expected<int, std::string> createExpectedByMove()
{
    fl::Expected<int, std::string> expected(42);
    fl::Expected<int, std::string> anotherExpected(std::move(expected));

    return anotherExpected;
}

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
        static constexpr std::string_view string_view{"42"};
        static constexpr auto unexpected = fl::Unexpected(string_view);
        static constexpr fl::Expected<int, std::string_view> expectedBox(unexpected);

        STATIC_REQUIRE(expectedBox.hasError());
    }

    SECTION("[Constexpr] From unexpected with correct value")
    {
        static constexpr std::string_view string_view{"42"};
        static constexpr auto unexpected = fl::Unexpected(string_view);
        static constexpr fl::Expected<int, std::string_view> expectedBox(unexpected);

        STATIC_REQUIRE(expectedBox.error() == string_view);
    }

    SECTION("Copy ctor")
    {
        fl::Expected<int, std::string> expected(42);
        fl::Expected<int, std::string> anotherExpected(expected);

        REQUIRE(expected == anotherExpected);
    }

    SECTION("[Constexpr] Copy ctor")
    {
        static constexpr fl::Expected<int, std::string> expected(42);
        static constexpr fl::Expected<int, std::string> anotherExpected(expected);

        STATIC_REQUIRE(expected == anotherExpected);
    }

    SECTION("Move ctor")
    {
        fl::Expected<int, std::string> expected(42);
        fl::Expected<int, std::string> anotherExpected(expected);
        fl::Expected<int, std::string> movedToExpected(std::move(anotherExpected));

        REQUIRE(expected == movedToExpected);
    }

    SECTION("[Constexpr] Move ctor")
    {
        static constexpr fl::Expected<int, std::string> expected(42);
        static constexpr auto createdExpected = test::details::createExpectedByMove();

        STATIC_REQUIRE(expected == createdExpected);
    }
}