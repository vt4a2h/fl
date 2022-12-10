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
#include <fl/concepts/concepts.hpp>
#include <fl/util/any_semigroup.hpp>
#include <fl/writer.hpp>

#include "catch.hpp"

namespace test_any_semigroup {

struct SemigroupString {
    using ValueType = std::string;

    [[nodiscard]]
    ValueType combine(const ValueType& v1, const ValueType& v2) const {
        ValueType result;

        result.reserve(v1.size() + v2.size());
        result.append(v1).append(v2);

        return v1 + v2;
    }

    [[nodiscard]]
    ValueType combine(ValueType&& v1, ValueType&& v2) const {
        return std::move(v1) + std::move(v2);
    }

    [[nodiscard]]
    ValueType combine(ValueType&& v1, const ValueType& v2) const {
        v1.reserve(v1.size() + v2.size());
        return std::move(v1).append(v2);
    }

    [[nodiscard]]
    ValueType combine(const ValueType& v1, ValueType&& v2) const {
        auto result = v1;
        result.reserve(v1.size() + v2.size());
        return std::move(result) + std::move(v2);
    }
};

}

TEST_CASE("Writer with any semigroup") {
    static_assert(fl::concepts::IsProbablySemigroup<test_any_semigroup::SemigroupString>);

    using LogType = std::string;
    using ValueType = int;
    using StringLogger = fl::Writer<LogType, ValueType>;

    const auto sg = fl::SemigroupWrapper<LogType>(test_any_semigroup::SemigroupString());

    SECTION("Temporary writer and string") {
        REQUIRE(StringLogger{"foo", 1}.tell("bar", sg).log() == std::string("foobar"));
    }

    SECTION("Temporary writer, regular string") {
        const std::string bar = "bar";
        REQUIRE(StringLogger{"foo", 1}.tell(bar, sg).log() == std::string("foobar"));
    }

    SECTION("Regular writer, temporary string") {
        StringLogger logger{"foo", 1};
        REQUIRE(logger.tell("bar", sg).log() == std::string("foobar"));
    }

    SECTION("Regular writer, regular string") {
        StringLogger logger{"foo", 1};
        const std::string bar = "bar";
        REQUIRE(logger.tell(bar, sg).log() == std::string("foobar"));
    }
}