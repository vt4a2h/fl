//
// MIT License
//
// Copyright (c) 2022-present Vitaly Fanaskov
//
// fl -- Writer "monad" for C++
// Project home: https://github.com/vt4a2h/fl
//
// See LICENSE file for the further details.
//
#include <fmt/format.h>

#include "writer_default_types.hpp"

#include "catch.hpp"

[[nodiscard]]
Value factorial_side_effect(Value i) {
    if (i == 0) {
        fmt::print("Factorial of 0 is 1\n");
        return 1;
    } else {
        auto ans = factorial_side_effect(i - 1) * i;
        fmt::print("Factorial of {} is {}\n", i, ans);
        return ans;
    }
}

[[nodiscard]]
Logger factorial_semi_functional(Value i) {
    if (i == 0) {
        return Logger{{"Factorial of 0 is 1"}, 1};
    } else {
        auto [l, v] = factorial_semi_functional(i - 1);
        auto ans = v * i;
        l.emplace_back(fmt::format("Factorial of {} is {}", i, ans));
        return Logger{std::move(l), ans};
    }
}

[[nodiscard]]
Logger better_factorial(Value i) {
    if (i == 0) {
        return Logger{{"Factorial of 0 is 1"}, 1};
    } else {
        auto r = better_factorial(i - 1).transform([&](Value v)
                                                   { return v * i; });
        return r.tell(fmt::format("Factorial of {} is {}", i, r.value()));
    }
}

auto logEntry(Value i, Value r) {
    return Log{fmt::format("Factorial of {} is {}", i, r), };
}

[[nodiscard]]
Logger factorial(Value i) {
    const auto mult = [&](auto v) { return v * i; };
    const auto addLogEntry = [&](auto a) { return Logger{logEntry(i, a), a}; };

    return (i == 0 ? Logger{{}, 1} : factorial(i - 1).transform(mult)).and_then(addLogEntry);
}

TEST_CASE("Side-effect factorial") {
    auto v = factorial_side_effect(5);
    REQUIRE(v == 120);
}

TEST_CASE("Factorial with writer") {
    const auto descriptionAndFunc = GENERATE(
        std::make_pair("Semi-functional factorial", &factorial_semi_functional),
        std::make_pair("More functional factorial", &better_factorial),
        std::make_pair("The best factorial", &factorial)
    );
    const auto [description, factorialFunc] = descriptionAndFunc;

    SECTION(description) {
        const Value maxFactorial = 5;
        const auto [log, result] = std::invoke(factorialFunc, maxFactorial);

        Value factorial = 1;
        for (Value i = 0; i <= maxFactorial; ++i) {
            factorial *= (i == 0 ? 1 : i);
            REQUIRE(fmt::format("Factorial of {} is {}", i, factorial) == log[static_cast<std::size_t>(i)]);
        }
        REQUIRE(result == factorial);
    }
}

