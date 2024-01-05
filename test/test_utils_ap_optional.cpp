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

#include <fl/utils/ap_optional.hpp>

#include <fmt/std.h>

TEST_CASE("Applicative for optional")
{
    SECTION("Regular args and function") {
        const auto add = [](int a, int b) { return a + b; };
        const int a = 1;
        const int b = 2;

        REQUIRE(fl::ap(add, a, b) == 3);
    }

    SECTION("Optional non-invocable") {
        const auto add = [](int a, const std::string &b) { return a + std::stoi(b); };

        const auto [arg1, arg2, expected] = GENERATE(table<std::optional<int>, std::optional<std::string>, std::optional<int>>({
            {std::make_optional<int>(1), std::make_optional<std::string>("2"), std::make_optional<int>(3)},
            {std::make_optional<int>(1), std::nullopt, std::nullopt},
            {std::nullopt, std::make_optional<std::string>("2"), std::nullopt},
        }));

        const auto actual = fl::ap(add, arg1, arg2);

        INFO(fmt::format("{} + {} = {}\n\texpected {}", arg1, arg2, actual, expected));
        REQUIRE(actual == expected);
    }

    SECTION("Optional invocable") {
        const auto add = [](std::optional<int> a, const std::optional<std::string> &b) -> std::optional<int> {
            if (!a || ! b) {
                return std::nullopt;
            } else {
                return *a + std::stoi(*b);
            }
        };

        const auto [arg1, arg2, expected] = GENERATE(table<std::optional<int>, std::optional<std::string>, std::optional<int>>({
            {std::make_optional<int>(1), std::make_optional<std::string>("2"), std::make_optional<int>(3)},
            {std::make_optional<int>(1), std::nullopt, std::nullopt},
            {std::nullopt, std::make_optional<std::string>("2"), std::nullopt},
        }));

        const auto actual = fl::ap(add, arg1, arg2);

        INFO(fmt::format("{} + {} = {}\n\texpected {}", arg1, arg2, actual, expected));
        REQUIRE(actual == expected);
    }

    SECTION("Optional partial ap") {
        const auto add = [](int a, const std::string &b) { return a + std::stoi(b); };

        const auto [arg1, arg2, expected] = GENERATE(table<std::optional<int>, std::string, std::optional<int>>({
            {std::make_optional<int>(1), "2", std::make_optional<int>(3)},
            {std::nullopt, "2", std::nullopt},
        }));

        const auto actual = fl::ap(add, arg1, arg2);

        INFO(fmt::format("{} + {} = {}\n\texpected {}", arg1, arg2, actual, expected));
        REQUIRE(actual == expected);
    }
}