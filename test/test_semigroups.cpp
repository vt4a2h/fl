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

#include <fl/semigroups/all.hpp>

#include <vector>


TEST_CASE("Combine") {
    SECTION("String") {
        fl::Semigroup<std::string> s;

        REQUIRE(s.combine("foo", "bar") == std::string("foobar"));
        REQUIRE(s.combine("baz", s.combine("foo", "bar")) == s.combine(s.combine("baz", "foo"), "bar"));
    }

    SECTION("String with view") {
        fl::Semigroup<std::string> sg;

        REQUIRE(sg.combine(std::string("foo"), std::string_view("bar")) == std::string("foobar"));
    }

    SECTION("String with char*") {
        fl::Semigroup<std::string> sg;

        REQUIRE(sg.combine(std::string("foo"), "bar") == std::string("foobar"));
    }

    SECTION("String with another string") {
        fl::Semigroup<std::string> sg;

        REQUIRE(sg.combine(std::string("foo"), std::string("bar")) == std::string("foobar"));
    }

    SECTION("Int") {
        fl::Semigroup<int> s;

        REQUIRE(s.combine(1, 2) == 3);
        REQUIRE(s.combine(1, s.combine(2, 3)) == s.combine(s.combine(1, 2), 3));
    }

    SECTION("Double") {
        fl::Semigroup<double> s;

        REQUIRE(s.combine(1., 2.) == 3.);
        REQUIRE(s.combine(1., s.combine(2., 3.)) == s.combine(s.combine(1., 2.), 3.));
    }

    SECTION("Vector tmp") {
        fl::Semigroup<std::vector<std::string>> s;

        REQUIRE(s.combine(std::vector<std::string>{"1", "2","3"}, std::vector<std::string>{"4", "5", "6"}) ==
                          std::vector<std::string>{"1", "2", "3", "4", "5", "6"});
    }

    SECTION("Vector const") {
        fl::Semigroup<std::vector<std::string>> s;

        const std::vector<std::string> v1{"1", "2", "3"};
        const std::vector<std::string> v2{"4", "5", "6"};
        REQUIRE(s.combine(v1, v2) == std::vector<std::string>{"1", "2", "3", "4", "5", "6"});
    }

    SECTION("Vector const-tmp") {
        fl::Semigroup<std::vector<std::string>> s;

        const std::vector<std::string> v1{"1", "2", "3"};
        REQUIRE(s.combine(v1, std::vector<std::string>{"4", "5", "6"}) ==
                std::vector<std::string>{"1", "2", "3", "4", "5", "6"});
    }

    SECTION("Vector tmp-const") {
        fl::Semigroup<std::vector<std::string>> s;

        const std::vector<std::string> v2{"4", "5", "6"};
        REQUIRE(s.combine(std::vector<std::string>{"1", "2","3"}, v2) ==
            std::vector<std::string>{"1", "2", "3", "4", "5", "6"});
    }

    SECTION("Set tmp") {
        fl::Semigroup<std::set<int>> s;

        REQUIRE(s.combine(std::set<int>{1, 2, 3}, std::set<int>{4, 5, 6}) == std::set<int>{1, 2, 3, 4, 5, 6});
    }

    SECTION("Set const") {
        fl::Semigroup<std::set<int>> s;

        const std::set<int> s1{1, 2, 3};
        const std::set<int> s2{4, 5, 6};

        REQUIRE(s.combine(s1, s2) == std::set<int>{1, 2, 3, 4, 5, 6});
    }

    SECTION("Set const-tmp") {
        fl::Semigroup<std::set<int>> s;

        const std::set<int> s1{1, 2, 3};

        REQUIRE(s.combine(s1, std::set<int>{4, 5, 6}) == std::set<int>{1, 2, 3, 4, 5, 6});
    }

    SECTION("Set tmp-const") {
        fl::Semigroup<std::set<int>> s;

        const std::set<int> s2{4, 5, 6};

        REQUIRE(s.combine(std::set<int>{1, 2, 3}, s2) == std::set<int>{1, 2, 3, 4, 5, 6});
    }

    SECTION("Vector with element") {
        fl::Semigroup<std::vector<int>> s;

        REQUIRE(s.combine(std::vector<int>{1, 2, 3}, 4) == std::vector<int>{1, 2, 3, 4});
    }

    SECTION("Set with element") {
        fl::Semigroup<std::set<int>> s;

        REQUIRE(s.combine(std::set<int>{1, 2, 3}, 4) == std::set<int>{1, 2, 3, 4});
        REQUIRE(s.combine(std::set<int>{1, 2, 4}, 3) == std::set<int>{1, 2, 3, 4});
    }
}