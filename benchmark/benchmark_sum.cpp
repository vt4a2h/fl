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

#include <filesystem>

#include <fmt/format.h>

#include <fl/semigroups/all.hpp>
#include <fl/writer.hpp>

#include "common/util.hpp"
#include "common/logging_fixture.hpp"

using Log = std::vector<std::string>;
using Val = std::uint64_t;
using Logger = fl::Writer<Log, Val>;

template<class L>
Val sum(Val upTo, L &logger) {
    Val result{};
    for (Val i = 0; i <= upTo; ++i) {
        ++result;
        logger->info(fmt::format("{} - {}", std::to_string(i), std::to_string(result)));
    }
    return result;
}

Logger sumWithLogger(Val upTo) {
    Logger result{};
    for (Val i = 0; i <= upTo; ++i) {
        result = std::move(result)
            .transform([&](Val v) { return ++v; })
            .and_then([&](Val v) { return Logger{{fmt::format("{} - {}", std::to_string(i), std::to_string(v))}, v}; });
    }
    return result;
}

TEST_CASE_METHOD(common::util::LoggingFixture, "Sum benchmark") {
    const auto value = GENERATE(10, 20, 30, 40, 50, 60, 70, 80, 90, 100);

    BENCHMARK(fmt::format("[Writer] Sum of {}", value))
    {
        const auto &[l, v] = sumWithLogger(value);
        for (const auto &s: l) {
            logger()->info(s);
        }
        return v;
    };
    BENCHMARK(fmt::format("[Just] Sum of {}", value))
    {
        return sum(value, logger());
    };

    SECTION(fmt::format("Sums of {} are equal", value)) {
        REQUIRE(sumWithLogger(value).value() == sum(value, logger()));
    }
}
