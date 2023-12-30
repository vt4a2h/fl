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
        const auto a = std::make_optional<int>(1);
        const auto b = std::make_optional<std::string>("2");

        REQUIRE(fl::ap(add, a, b) == 3);
    }

    SECTION("Optional invocable") {
        const auto add = [](std::optional<int> a, const std::optional<std::string> &b) { return *a + std::stoi(*b); };
        const auto a = std::make_optional<int>(1);
        const auto b = std::make_optional<std::string>("2");

        REQUIRE(fl::ap(add, a, b) == 3);
    }

    SECTION("Optional partial ap") {
        const auto add = [](int a, const std::string &b) { return a + std::stoi(b); };
        const auto a = std::make_optional<int>(1);
        const auto b = std::string{"2"};

        REQUIRE(fl::ap(add, a, b) == 3);
    }
}