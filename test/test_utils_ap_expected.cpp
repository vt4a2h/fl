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
#include "catch.hpp"

#include <fl/utils/ap_expected.hpp>

#include <fmt/std.h>

#include <range/v3/all.hpp>

TEST_CASE("Expected concepts") {
    SECTION("Detected") {
        STATIC_REQUIRE(fl::details::IsExpected<std::expected<int, int>>);
    }

    SECTION("Not detected") {
        STATIC_REQUIRE_FALSE(fl::details::IsExpected<std::pair<int, int>>);
    }

    SECTION("All same error") {
        STATIC_REQUIRE(
            fl::details::AllSameErr<
                std::expected<int, int>,
                std::expected<double, int>,
                std::expected<std::string, int>
            >
        );
    }

    SECTION("Some different error types") {
        STATIC_REQUIRE_FALSE(
            fl::details::AllSameErr<
                std::expected<int, int>,
                std::expected<double, double>,
                std::expected<std::string, int>
            >
        );
    }

    SECTION("AllSameErr with a single type") {
        STATIC_REQUIRE(fl::details::AllSameErr<std::expected<int, int>>);
    }

    SECTION("AllSameErr with a no types. False, because we need at least one expected here") {
        constexpr auto empty_lambda = []<class... Ts>(Ts&&...) constexpr{ return fl::details::AllSameErr<Ts...>; };
        STATIC_REQUIRE_FALSE(empty_lambda());
    }
}

TEST_CASE("Expected app propagates the first error") {
    using Expected = std::expected<int, std::string>;

    const auto [arg1, arg2, arg3, expectedError] = GENERATE(table<Expected, Expected, Expected, std::optional<std::string>>({
        {Expected(std::unexpected(std::string("first_error"))), Expected(1),
         Expected(std::unexpected(std::string("second_error"))), std::make_optional(std::string("first_error"))},
        {Expected(1), Expected(std::unexpected(std::string("first_error"))),
         Expected(std::unexpected(std::string("second_error"))), std::make_optional(std::string("first_error"))},
        {Expected(1), Expected(1),
         Expected(std::unexpected(std::string("first_error"))), std::make_optional(std::string("first_error"))},
        {Expected(1), Expected(1), Expected(1), std::nullopt},
    }));

    const auto actualFirstError = fl::details::firstError(arg1, arg2, arg3);

    REQUIRE(actualFirstError == expectedError);
}

TEST_CASE("Applicative for expected")
{
    using Expected = std::expected<int, std::string>;

    SECTION("Just invocable")
    {
        const auto add = [](int a, int b) { return a + b; };

        REQUIRE(fl::ap<std::string>(add, 1, 2) == std::expected<int, std::string>{3});
    }

    SECTION("All arguments are expected values") {
        const auto add = [](int a, int b) { return a + b; };

        const auto [a, b, result] = GENERATE(table<Expected, Expected, Expected>({
            {Expected(1), Expected(2), Expected(3)},
            {Expected(std::unexpected(std::string("first_error"))),
             Expected(2),
             Expected(std::unexpected(std::string("first_error")))},
            {Expected(1),
             Expected(std::unexpected(std::string("second_error"))),
             Expected(std::unexpected(std::string("second_error")))},
            {Expected(std::unexpected(std::string("first_error"))),
             Expected(std::unexpected(std::string("second_error"))),
             Expected(std::unexpected(std::string("first_error")))},
        }));

        REQUIRE(fl::ap(add, a, b) == result);
    }


//    SECTION("Optional invocable") {
//        const auto add = [](std::optional<int> a, const std::optional<std::string> &b) -> std::optional<int> {
//            if (!a || ! b) {
//                return std::nullopt;
//            } else {
//                return *a + std::stoi(*b);
//            }
//        };
//
//        const auto [arg1, arg2, expected] = GENERATE(table<std::optional<int>, std::optional<std::string>, std::optional<int>>({
//            {std::make_optional<int>(1), std::make_optional<std::string>("2"), std::make_optional<int>(3)},
//            {std::make_optional<int>(1), std::nullopt, std::nullopt},
//            {std::nullopt, std::make_optional<std::string>("2"), std::nullopt},
//        }));
//
//        const auto actual = fl::ap(add, arg1, arg2);
//
//        INFO(fmt::format("{} + {} = {}\n\texpected {}", arg1, arg2, actual, expected));
//        REQUIRE(actual == expected);
//    }
//
//    SECTION("Optional partial ap") {
//        const auto add = [](int a, const std::string &b) { return a + std::stoi(b); };
//
//        const auto [arg1, arg2, expected] = GENERATE(table<std::optional<int>, std::string, std::optional<int>>({
//            {std::make_optional<int>(1), "2", std::make_optional<int>(3)},
//            {std::nullopt, "2", std::nullopt},
//        }));
//
//        const auto actual = fl::ap(add, arg1, arg2);
//
//        INFO(fmt::format("{} + {} = {}\n\texpected {}", arg1, arg2, actual, expected));
//        REQUIRE(actual == expected);
//    }
}
