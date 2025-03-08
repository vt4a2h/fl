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