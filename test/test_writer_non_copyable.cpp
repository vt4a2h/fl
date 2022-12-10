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
#include "catch.hpp"

#include <string>

#include <fl/semigroups/semigroup.hpp>
#include <fl/writer.hpp>

class NonCopyableString : public std::string {
public:
    NonCopyableString() = default;
    explicit NonCopyableString(const std::string& s) : std::string(s) {}

    NonCopyableString(NonCopyableString &&) noexcept = default;
    NonCopyableString &operator=(NonCopyableString &&) noexcept = default;

    explicit NonCopyableString(const NonCopyableString&) = delete;
    NonCopyableString &operator=(const NonCopyableString&) = delete;
};

namespace fl {

template<>
struct Semigroup<NonCopyableString> {
    [[nodiscard]]
    NonCopyableString combine(NonCopyableString&& v1, NonCopyableString&& v2) const {
        v1.reserve(v1.size() + v2.size() + 1);
        v1.append("-").append(v2);

        return v1;
    }
};

}

TEST_CASE("Writer with non-copyable log type") {
    using Value = std::uint64_t;
    using Log = NonCopyableString;
    using Logger = fl::Writer<Log, Value>;

    SECTION("Tell") {
        auto log = Logger{}.tell(NonCopyableString("foo")).tell(NonCopyableString("bar")).log();
        REQUIRE(log == NonCopyableString("-foo-bar"));
    }

    SECTION("Transform") {
        const auto [l, v] = Logger{NonCopyableString{"foo"}, 1}
            .tell(NonCopyableString("bar"))
            .transform([](Value val) { return ++val; })
            .as_tuple();

        REQUIRE(l == NonCopyableString("foo-bar"));
        REQUIRE(v == Value(2));
    }

    SECTION("And then") {
        const auto [l, v] = Logger{NonCopyableString{"foo"}, 1}
            .and_then([](Value val) { return Logger{NonCopyableString("bar"), val + 1}; })
            .as_tuple();

        REQUIRE(l == NonCopyableString("foo-bar"));
        REQUIRE(v == Value(2));
    }

    SECTION("Apply") {
        using LoggerWithF = fl::Writer<Log, std::function<Value(Value)>>;

        const auto [l, v] = Logger{NonCopyableString{"foo"}, 10}
            .apply(LoggerWithF{NonCopyableString{"bar"}, [](Value val) { return val * 2; }})
            .as_tuple();

        REQUIRE(l == NonCopyableString{"bar-foo"});
        REQUIRE(v == 20);
    }
}