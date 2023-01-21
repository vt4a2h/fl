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
#define CATCH_CONFIG_USE_ASYNC
#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include "catch.hpp"

#include <fmt/format.h>

#include <fl/semigroups/all.hpp>
#include "fl/writer/writer.hpp"

#include "common/logging_fixture.hpp"

using Log = std::vector<std::string>;
using Val = std::uint64_t;
using Logger = fl::Writer<Log, Val>;

[[nodiscard]]
Logger factorial(Val i) {
    const auto mult = [&](Val v) { return v * i; };
    const auto tell = [&](Val ans) { return Logger{{fmt::format("Factorial of {} is {}", i, ans)}, ans}; };
    return (i == 0 ? Logger{{}, 1} : factorial(i - 1).transform(mult)).and_then(tell);
}

// Add some logging
template <class L>
[[nodiscard]]
Val just_factorial(Val i, L &logger) {
    if (i == 0) {
        logger->info("Factorial of 0 is 1");
        return 1;
    } else {
        const auto ans = just_factorial(i - 1, logger) * i;
        logger->info("Factorial of {} is {}", i, ans);
        return ans;
    }
}

Val factorial_no_logging(Val i) {
    return (i == 0 ? 1 : factorial_no_logging(i - 1) * i);
}

TEST_CASE_METHOD(common::util::LoggingFixture, "Factorial benchmark") {
    const auto value = GENERATE(5, 10, 15, 20);

    static_assert(std::movable<Logger>);

    BENCHMARK(fmt::format("[Writer] Factorial of {}", value)) {
        const auto &[l, v] = factorial(value);
        for (const auto &s : l) {
            logger()->info(s);
        }
        return v;
    };
    BENCHMARK(fmt::format("[Just] Factorial of {}", value)) {
        return just_factorial(value, logger());
    };

    SECTION(fmt::format("Factorials of {} are equal", value)) {
        REQUIRE(factorial(value).value() == just_factorial(value, logger()));
    }

    SECTION(fmt::format("All factorials of {} are equal", value)) {
        REQUIRE(factorial(value).value() == factorial_no_logging(value));
    }
}

