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

#include <fmt/format.h>

using namespace fl;

TEMPLATE_TEST_CASE_SIG("Valid arguments for applicative", "",
    ((class Error, class Arg1, class Arg2, class Arg3, bool Valid), Error, Arg1, Arg2, Arg3, Valid),
     (std::string, expected<int, std::string>, expected<double, std::string>, expected<bool, std::string>, true),
     (std::string, expected<int, std::string>, double, expected<bool, std::string>, true),
     (std::string, int, double, expected<bool, std::string>, true),
     (std::string, int, double, bool, true),
     (std::string, expected<int, std::string_view>, expected<double, std::string>, expected<bool, std::string>, false),
     (std::string, expected<int, std::string_view>, expected<double, std::string_view>, expected<bool, std::string_view>, false),
     (std::string, int, double, expected<bool, std::string_view>, false)
)
{
    STATIC_REQUIRE(detail::ValidApArgs<Error, Arg1, Arg2, Arg3> == Valid);
}

int add_v1(int a1, int a2) { return a1 + a2; }
int add_v2(int a1, expected<int, std::string> a2) { return a1 + std::get<int>(a2); }
int add_v3(expected<int, std::string> a1, expected<int, std::string> a2) { return std::get<int>(a1) + std::get<int>(a2); }
expected<int, std::string> add_v4(expected<int, std::string> a1, expected<int, std::string> a2)
{ return std::get<int>(a1) + std::get<int>(a2); }
expected<int, std::string> add_v5(int a1, expected<int, std::string> a2)
{ return a1 + std::get<int>(a2); }

int add_invalid_v1(int a1, expected<int, std::string_view> a2) { return a1 + std::get<int>(a2); }
int add_invalid_v2(expected<int, std::string_view> a1, expected<int, std::string_view> a2)
{ return std::get<int>(a1) + std::get<int>(a2); }
expected<int, std::string_view> add_invalid_v3(expected<int, std::string_view> a1, expected<int, std::string_view> a2)
{ return std::get<int>(a1) + std::get<int>(a2); }
expected<int, std::string_view> add_invalid_v4(int a1, int a2) { return a1 + a2; }

TEMPLATE_TEST_CASE_SIG("Invokable function for applicative", "",
    ((class F, class Error, class Arg1, class Arg2, bool Valid), F, Error, Arg1, Arg2, Valid),
     (decltype(&add_v1), std::string, int, int, true),
     (decltype(&add_v2), std::string, int, expected<int, std::string>, true),
     (decltype(&add_v3), std::string, expected<int, std::string>, expected<int, std::string>, true),
     (decltype(&add_v4), std::string, expected<int, std::string>, expected<int, std::string>, true),
     (decltype(&add_v5), std::string, int, expected<int, std::string>, true),

     (decltype(&add_invalid_v1), std::string, int, expected<int, std::string_view>, false),
     (decltype(&add_invalid_v2), std::string, expected<int, std::string_view>, expected<int, std::string_view>, false),
     (decltype(&add_invalid_v3), std::string, expected<int, std::string_view>, expected<int, std::string_view>, false),
     (decltype(&add_invalid_v4), std::string, int, int, false)
    )
{
    STATIC_REQUIRE(detail::ApInvocable<F, Error, Arg1, Arg2> == Valid);
}

TEST_CASE("First error")
{
    SECTION("No error")
    {
        const auto actualResult = detail::firstError<std::string>(3, 4., true);
        const std::optional<std::string> expectedResult;

        REQUIRE(actualResult == expectedResult);
    }

    SECTION("Only one error")
    {
        const auto actualResult =
            detail::firstError<std::string>(expected<int, std::string>{"3"}, 4., true);
        const auto expectedResult = std::make_optional<std::string>("3");

        REQUIRE(actualResult == expectedResult);
    }

    SECTION("First errors of several")
    {
        const auto actualResult =
            detail::firstError<std::string>(expected<int, std::string>{"3"}, expected<int, std::string>{"4."}, true);
        const auto expectedResult = std::make_optional<std::string>("3");

        REQUIRE(actualResult == expectedResult);
    }

    SECTION("Error not at the first place")
    {
        const auto actualResult =
            detail::firstError<std::string>(expected<int, std::string>{3}, expected<int, std::string>{"4."}, true);
        const auto expectedResult = std::make_optional<std::string>("4.");

        REQUIRE(actualResult == expectedResult);
    }
}

void noExpectedArgs(int, const std::string&){ /*nothing*/ }
expected<void, std::string> noExpectedArgsRe(int, const std::string&) { return {}; }
void oneExpectedArg(const expected<int, std::string>&, const std::string&){ /*nothing*/ }
expected<void, std::string> oneExpectedArgRe(const expected<int, std::string>&, const std::string&){ return {}; }

TEMPLATE_TEST_CASE_SIG("Result is valid", "",
    ((class ActualT, class ExpectedT, bool Same), ActualT, ExpectedT, Same),
     (detail::ApInvocableResult<decltype(&noExpectedArgs), std::string, int, std::string>, expected<void, std::string>, true),
     (detail::ApInvocableResult<decltype(&noExpectedArgsRe), std::string, int, std::string>, expected<void, std::string>, true),
     (detail::ApInvocableResult<decltype(&oneExpectedArg), std::string, expected<int, std::string>, std::string>, expected<void, std::string>, true),
     (detail::ApInvocableResult<decltype(&oneExpectedArgRe), std::string, expected<int, std::string>, std::string>, expected<void, std::string>, true)
)
{
    STATIC_REQUIRE(std::is_same_v<ActualT, ExpectedT> == Same);
}

using Expected = expected<int, std::string>;

template <class E, class V>
void checkError(E &&result, V &&expectedError)
{
    std::ignore = result.and_then([](const auto &v) -> Expected {
        FAIL(fmt::format("Get a value {} instead of an error.", v));
        return {};
    }).or_else([&expectedError](const auto& actualError) -> Expected {
        REQUIRE(expectedError == actualError);
        return {};
    });
}

TEST_CASE("Error if expected contains error")
{
    const std::string expectedError{"error message"};

    Expected e{expectedError};

    const auto result = e.ap([](int, int) { return 42; }, 1);

    checkError(result, expectedError);
}

TEST_CASE("First error [invoked]")
{
    const auto&[arg2, arg3, expectedError] = GENERATE(table<Expected, Expected, std::string>({
        {Expected{"Wrong arg1"}, Expected{3}, "Wrong arg1"},
        {Expected{2}, Expected{"Wrong arg2"}, "Wrong arg2"},
    }));

    Expected e{1};

    const auto add = [](int a1, int a2, int a3) { return a1 + a2 + a3; };

    const auto result = e.ap(add, arg2, arg3);

    checkError(result, expectedError);
}

template <class E, class V>
void checkValue(E &&result, V &&expectedValue)
{
    std::ignore = result.and_then([&expectedValue](const auto &actualValue) -> Expected {
        REQUIRE(actualValue == expectedValue);
        return {};
    }).or_else([](const auto& e) -> Expected {
        FAIL(fmt::format("Get an error {} instead of a value.", e));
        return {};
    });
}

TEST_CASE("Value [invoked]")
{
    const Expected e{1};

    SECTION("All regular values")
    {
        const auto result = e.ap([](int a1, int a2) { return a1 + a2; }, 1);
        checkValue(result, 2);
    }

    SECTION("All expected values")
    {
        const auto result = e.ap([](int a1, int a2, int a3) { return a1 + a2 + a3; }, Expected{1}, Expected{1});
        checkValue(result, 3);
    }

    SECTION("Some expected values")
    {
        const auto result = e.ap([](int a1, int a2, int a3) { return a1 + a2 + a3; }, 1, Expected{1});
        checkValue(result, 3);
    }
}