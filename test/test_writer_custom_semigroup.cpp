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
#include "writer_default_types.hpp"

#include "catch.hpp"

struct MyString : public std::string{ using std::string::string; };

namespace fl {

template<>
struct Semigroup<MyString> {
    [[nodiscard]]
    MyString combine(const MyString& v1, const MyString& v2) const {
        MyString result;

        result.reserve(v1.size() + v2.size() + 1);
        result.append(v1).append("\n").append(v2);

        return result;
    }
};

}

TEST_CASE("Writer with custom semigroup") {

    SECTION("custom tell") {
        auto customWriter = fl::Writer<MyString, int>{"foo", 1};

        auto actual = customWriter.tell("bar").log();
        auto expected = MyString("foo\nbar");

        REQUIRE(actual == expected);
    }

}
