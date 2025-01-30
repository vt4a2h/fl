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

#include <expected>

using namespace fl::experimental;

struct Foo {};
struct Bar {};

struct ConvertableFromFoo{ explicit(false) ConvertableFromFoo(const Foo&) {} };
struct ExplicitlyConvertableFromFoo{
    ExplicitlyConvertableFromFoo() = default;
    explicit ExplicitlyConvertableFromFoo(const Foo&) {}
};

struct NonDefaultConstructable { NonDefaultConstructable() = delete; };

TEST_CASE("Create")
{
    SECTION("Has default constructed value by default")
    {
        expected<double, std::string> e;
        REQUIRE(e);
    }

    SECTION("Has value when set")
    {
        const expected<double, std::string> e{42.};
        REQUIRE(e.has_value());
    }

    SECTION("Has error when set")
    {
        const expected<double, std::string> e{"error"};
        REQUIRE(e.has_error());
    }

    SECTION("User types")
    {
        using Expected  = expected<Foo, Bar>;

        const auto&[expected, hasValue] = GENERATE(table<Expected, bool>({
            {Expected{Foo{}}, true},
            {Expected{Bar{}}, false},
        }));

        REQUIRE(expected.has_value() == hasValue);
    }
}

TEST_CASE("Comparable")
{
    using Expected = expected<int, std::string>;
    const auto [lhs, rhs, equal] = GENERATE(table<Expected, Expected, bool>({
        {Expected{42}, Expected{42}, true},
        {Expected{}, Expected{}, true},
        {Expected{"42"}, Expected{"42"}, true},
        {Expected{42}, Expected{"42"}, false},
        {Expected{"42"}, Expected{42}, false},
    }));

    REQUIRE((lhs == rhs) == equal);
}

template <class Value, class Error>
concept CanCreateExpected = requires {
    expected<Value, Error>();
};

TEMPLATE_TEST_CASE_SIG("Can define a type", "",
                       ((class V, class E, bool C), V, E, C),
                       (Foo, Bar, true),
                       (Bar, Foo, true),
                       (Foo, Foo, false),
                       (Foo, ConvertableFromFoo, false),
                       (ConvertableFromFoo, Foo, false),
                       (NonDefaultConstructable, Bar, false),
                       (Bar, NonDefaultConstructable, true),
                       (ExplicitlyConvertableFromFoo, Foo, true)
                       )
{
    STATIC_REQUIRE(CanCreateExpected<V, E> == C);
}

TEST_CASE("And then")
{
    SECTION("Invoked") {
        const expected<Foo, std::string> e;
        const auto result = e.and_then([](const auto &) -> expected<Bar, std::string> { return {}; });

        REQUIRE(result.has_value());
    }
}