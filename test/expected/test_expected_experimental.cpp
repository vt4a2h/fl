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

struct ConvertableFromFoo{
    ConvertableFromFoo() = default;
    explicit(false) ConvertableFromFoo(const Foo&) {}
};
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
        using Expected = expected<Foo, std::string>;

        const auto [expected, expectedToBeInvoked] =
            GENERATE(table<Expected, bool>({
                {Expected{Foo{}}, true},
                {Expected{"123"}, false},
            }));

        bool invoked{};
        std::ignore = expected.and_then([&invoked](const auto &) -> Expected {
            invoked = true;
            return {};
        });

        REQUIRE(invoked == expectedToBeInvoked);
    }

    SECTION("Type can be re-mapped")
    {
        const expected<Foo, std::string> e;

        const int expectedNumber = 42;
        const auto actualResult =
            e.and_then([expectedNumber](const auto&) -> expected<int, std::string> { return expectedNumber; });

        std::ignore = actualResult
            .and_then([](const auto& actualNumber) -> expected<int, std::string> {
                REQUIRE(actualNumber == expectedNumber);
                return {};
            })
            .or_else([](const auto&) -> expected<int, std::string> {
                FAIL();
                return {};
            });
    }
}

TEST_CASE("Or else")
{
    SECTION("Invoked") {
        using Expected = expected<Foo, std::string>;

        const auto [expected, expectedToBeInvoked] =
            GENERATE(table<Expected, bool>({
                {Expected{Foo{}}, false},
                {Expected{"123"}, true},
            }));

        bool invoked{};
        std::ignore = expected.or_else([&invoked](const auto &) -> Expected {
            invoked = true;
            return {};
        });

        REQUIRE(invoked == expectedToBeInvoked);
    }

    SECTION("Type can be re-mapped")
    {
        const expected<std::string, Foo> e{Foo{}};

        const int expectedNumber = 42;
        const auto actualResult =
            e.or_else([expectedNumber](const auto&) -> expected<std::string, int> { return expectedNumber; });

        std::ignore = actualResult
            .or_else([](const auto& actualNumber) -> expected<std::string, int> {
                REQUIRE(actualNumber == expectedNumber);
                return actualNumber;
            })
            .and_then([](const auto&) -> expected<std::string, int> {
                FAIL();
                return {};
            });
    }
}

TEST_CASE("Transform")
{
    SECTION("Invoked") {
        using Expected = expected<Foo, std::string>;

        const auto [expected, expectedToBeInvoked] =
            GENERATE(table<Expected, bool>({
                {Expected{Foo{}}, true},
                {Expected{"123"}, false},
            }));

        bool invoked{};
        std::ignore = expected.transform([&invoked](const auto &) {
            invoked = true;
            return invoked;
        });

        REQUIRE(invoked == expectedToBeInvoked);
    }

    SECTION("Type can be changed")
    {
        std::ignore = expected<int, std::string>{}
            .transform([](const auto&) {
                SUCCEED();
                return Foo{};
            })
            .or_else([](const std::string&) -> expected<Foo, std::string> {
                FAIL();
                return {};
            });
    }
}

TEST_CASE("Transform error")
{
    SECTION("Invoked") {
        using Expected = expected<Foo, std::string>;

        const auto [expected, expectedToBeInvoked] =
            GENERATE(table<Expected, bool>({
                {Expected{Foo{}}, false},
                {Expected{"123"}, true},
            }));

        bool invoked{};
        std::ignore = expected.transform_error([&invoked](const auto &) {
            invoked = true;
            return invoked;
        });

        REQUIRE(invoked == expectedToBeInvoked);
    }

    SECTION("Type can be changed")
    {
        std::ignore = expected<int, std::string>{"123"}
            .transform_error([](const auto&) {
                SUCCEED();
                return Foo{};
            })
            .transform([](const int&) -> int {
                FAIL();
                return {};
            });
    }
}

template <class Expected, class NewType>
concept CanInvokeRebind = requires(Expected e) {
    e.template rebind<NewType>();
};

TEMPLATE_TEST_CASE_SIG("Rebind", "",
                       ((class E, class N, bool C), E, N, C),
                        (expected<Foo, std::string>, ConvertableFromFoo, true),
                        (expected<Foo, std::string>, ExplicitlyConvertableFromFoo, false),
                        (expected<int, std::string>, double, true),
                        (expected<Foo, std::string>, double, false)
                       )
{
    STATIC_REQUIRE(CanInvokeRebind<E, N> == C);
}