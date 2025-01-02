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

#include <fl/expected/expected.hpp>

TEST_CASE("Convertable to variant")
{
    using Expected = fl::Expected<std::string_view, int>;

    constexpr Expected expected{"42"};
    constexpr typename Expected::Data data(expected.as_variant());

    STATIC_REQUIRE(expected.value() == std::get<0>(data));
}

template<class... Ts>
struct overloaded : Ts... { using Ts::operator()...; };

TEST_CASE("Use in visit")
{
    using Expected = fl::Expected<std::string, int>;
    using Data = typename Expected::Data;

    const auto &[expectedObj, expectedResult] = GENERATE(table<Expected, Data>({
       {Expected{"Nothing"}, Data{"Something"}},
       {Expected{fl::Unexpected{42}}, Data{42}},
    }));

    const auto actualResult = std::visit(overloaded{
        [&expectedResult](const std::string &/*value*/) { return expectedResult; },
        [&expectedResult](const int &/*error*/) { return expectedResult; },
    }, expectedObj.as_variant());

    REQUIRE(expectedResult == actualResult);
}