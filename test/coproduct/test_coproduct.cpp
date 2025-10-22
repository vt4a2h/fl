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

#include <fl/coproduct/coproduct.hpp>

TEST_CASE("Create coproduct")
{
    const fl::Coproduct<int> v{42};
    REQUIRE(fl::get_value<int>(v) == 42);
}

TEMPLATE_TEST_CASE_SIG("Coproduct arity", "",
                       ((class Coproduct, std::size_t Arity), Coproduct, Arity),
                       (fl::Coproduct<int>, 1),
                       (fl::Coproduct<int, std::string>, 2))
{
    STATIC_REQUIRE(fl::arity<Coproduct> == Arity);
}

TEMPLATE_TEST_CASE_SIG("Coproduct holds value of T - 1", "",
                       ((class T, bool Holds), T, Holds),
                       (int, true),
                       (std::string, false))
{
    const fl::Coproduct<int, std::string> v{42};

    REQUIRE(fl::holds_value_of_type<T>(v) == Holds);
}

TEST_CASE("Coproduct match")
{
    constexpr int expectedValue = 42;
    const fl::Coproduct<int, std::string> v{expectedValue};

    fl::match(v, [](int v) { REQUIRE(v == expectedValue); },
              [](const std::string&) { FAIL("Matched wrong value"); });
}
