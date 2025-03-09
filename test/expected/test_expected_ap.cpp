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