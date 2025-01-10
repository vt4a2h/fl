//
// MIT License
//
// Copyright (c) 2025-present Vitaly Fanaskov
//
// fl -- Functional tools for C++
// Project home: https://github.com/vt4a2h/fl
//
// See LICENSE file for the further details.
//
#include "catch.hpp"

#include <fl/utils/match.hpp>

#include <fmt/format.h>

template<class... Ts>
struct overloaded : Ts... { using Ts::operator()...; };

TEST_CASE("Invoked for all alternatives")
{
    auto intHandler = [](int v){ fmt::println("int: {}", v); };
    auto stringHandler = [](const std::string &v){ fmt::println("string: {}", v); };
    auto doubleHandler = [](double v){ fmt::println("double: {}", v); };

    std::variant<int, std::string, double> v = 42;

    fl::match(v,  stringHandler, intHandler, doubleHandler);
}