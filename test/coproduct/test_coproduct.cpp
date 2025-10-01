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

TEST_CASE("Create coproduct - 1")
{
    const fl::Coproduct<int> v{.value = 42};
    REQUIRE(v.value == 42);
}

TEST_CASE("Create coproduct - 2 - 0")
{
    const fl::Coproduct<int, std::string> v{.value = {.v0 = 42}, .is_v0 = true};
    REQUIRE(v.value.v0 == 42);
}

TEST_CASE("Create coproduct - 2 - 1")
{
    const fl::Coproduct<int, std::string> v{.value = {.v1 = "42"}, .is_v0 = false};
    REQUIRE(v.value.v1 == "42");
}

TEMPLATE_TEST_CASE_SIG("Coproduct arity", "",
                       ((class Coproduct, std::size_t Arity), Coproduct, Arity),
                       (fl::Coproduct<int>, 1),
                       (fl::Coproduct<int, std::string>, 2))
{
    STATIC_REQUIRE(fl::arity<Coproduct> == Arity);
}
