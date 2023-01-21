//
// MIT License
//
// Copyright (c) 2023-present Vitaly Fanaskov
//
// fl -- Writer "monad" for C++
// Project home: https://github.com/vt4a2h/fl
//
// See LICENSE file for the further details.
//
#include "catch.hpp"

#include "writer_default_types.hpp"
#include "writer_utility_types.hpp"

#include <fl/writer/lazy_operations.hpp>

TEST_CASE("Use lazy evaluation") {
    using namespace fl;

    using namespace writer_tests_util;
    using CopyOnlyLogger = fl::Writer<NonMovableString, NonMovableString>;
    using MoveOnlyLogger = fl::Writer<NonCopyableString, NonCopyableString>;

    SECTION("Tell operation on temporary objects") {
        auto w = Logger{} | tell(Log{"123"});
        auto r = w.eval();

        REQUIRE(r.log() == Log{"123"});
    }

    SECTION("Tell operation on const objects") {
        const auto l = Logger{};
        const auto t = tell(Log{"123"});
        auto w = l | t;
        auto r = w.eval();

        REQUIRE(r.log() == Log{"123"});
    }

    SECTION("Tell operation for copy-only temporary objects") {
        auto w = CopyOnlyLogger{NonMovableString{"123"}, {}} | tell(NonMovableString{"123"});
        auto r = *w;

        REQUIRE(r.log() == NonCopyableString{"123-123"});
    }

    SECTION("Tell operation for copy-only const objects") {
        const auto l = CopyOnlyLogger{NonMovableString{"123"}, {}};
        const auto t = tell(NonMovableString{"123"});
        auto w = l | t;
        auto r = *w;

        REQUIRE(r.log() == NonCopyableString{"123-123"});
    }

    SECTION("Tell operation for move-only temporary objects") {
        auto w = MoveOnlyLogger{NonCopyableString{"123"}, {}} | tell(NonCopyableString{"123"});
        auto r = (*w).log();

        REQUIRE(r == NonCopyableString{"123-123"});
    }

    SECTION("Tell operation for move-only objects") {
        auto l = MoveOnlyLogger(NonCopyableString{"123"}, {});
        auto t = tell(NonCopyableString{"123"});
        auto w = std::move(l) | std::move(t); // Must move no copy ctor!
        auto r = (*w).log();

        REQUIRE(r == NonCopyableString{"123-123"});
    }

    SECTION("Several tell operations") {
        auto w = Logger{} | tell(Log{"123"}) | tell(Log{"456"}) | tell(Log{"678"});
        auto r = w.eval();

        REQUIRE(r.log() == Log{"123", "456", "678"});
    }

    SECTION("Several tell operations for constant objects") {
        const auto l = Logger{};
        const auto t = tell(Log{"123"});
        auto w = l | t | tell(Log{"456"}) | tell(Log{"678"});
        auto r = w.eval();

        REQUIRE(r.log() == Log{"123", "456", "678"});
    }

    SECTION("Transform operation") {
        auto transformed = Logger{{}, 1} | transform([](auto v) { return ++v; });
        auto result = transformed.eval();

        REQUIRE(result.value() == 2);
    }

    SECTION("Transform operation + tell") {
        auto transformed = Logger{{}, 1} |
            transform([](auto v) { return ++v; }) |
            tell(Log{"123"});
        auto result = transformed.eval();

        REQUIRE(result.value() == 2);
        REQUIRE(result.log() == Log{"123"});
    }

    SECTION("And then operation") {
        auto next = Logger{{}, 1} | and_then([](auto v) { return Logger{{"123"}, v + 3}; });
        auto result = next.eval();

        REQUIRE(result.log() == Log{"123"});
        REQUIRE(result.value() == 4);
    }

    SECTION("Test eval") {
        const auto result = Logger{{}, 1} | transform([](auto v) { return ++v; }) | eval;

        REQUIRE(result.value() == 2);
    }

    SECTION("Operation is really lazy") {
        int counter{};
        const auto result = Logger{{}, 1}
            | transform([&](auto v) { return ++counter, ++v; })
            | transform([&](auto v) { return ++counter, ++v; });
        (void)&result;

        REQUIRE(counter == 0);
    }
}