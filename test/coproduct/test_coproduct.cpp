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
    const fl::Coproduct<int, std::string> v{.value = {._0 = 42}, .is_0 = true};
    REQUIRE(v.value._0 == 42);
}

TEST_CASE("Create coproduct - 2 - 1")
{
    const fl::Coproduct<int, std::string> v{.value = {._1 = "42"}, .is_0 = false};
    REQUIRE(v.value._1 == "42");
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
    const fl::Coproduct<int> v{.value = 42};

    REQUIRE(fl::holds_value_of_type<T>(v) == Holds);
}

namespace
{
    using Coproduct = fl::Coproduct<int, std::string>;
    auto make = []<class Val>(const Val& v)
    {
        if constexpr (std::is_same_v<std::remove_cvref_t<Val>, Coproduct::value_t_0>)
        {
            return Coproduct{.value = {._0 = v}, .is_0 = true};
        }
        else
        {
            return Coproduct{.value = {._1 = v}, .is_0 = false};
        }
    };
}

TEMPLATE_TEST_CASE_SIG("Coproduct holds value of T - 2", "",
                       ((class V, class T, bool Holds), V, T, Holds),
                       (int, int, true),
                       (std::string, std::string, true),
                       (int, std::string, false),
                       (std::string, int, false)
)
{
    const auto v = make(V{});

    REQUIRE(fl::holds_value_of_type<T>(v) == Holds);
}
