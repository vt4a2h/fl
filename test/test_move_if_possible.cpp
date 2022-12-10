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

#include <fl/util/move_if_possible.hpp>

namespace fl_test {
struct CopyOnly : public std::string {
    CopyOnly() = default;
    CopyOnly(const CopyOnly &) : std::string() { this->assign("copied-ctor"); }
    CopyOnly(CopyOnly &&) = delete;

    CopyOnly& operator=(const CopyOnly &) { this->assign("copied-op"); return *this; }
    CopyOnly& operator=(CopyOnly &&) noexcept = delete;
};

struct MoveOnly : public std::string {
    MoveOnly() = default;
    MoveOnly(const MoveOnly &) = delete;
    MoveOnly(MoveOnly &&) noexcept { this->assign("moved-ctor"); }

    MoveOnly& operator=(const MoveOnly &) = delete;
    MoveOnly& operator=(MoveOnly &&) noexcept { this->assign("moved-op"); return *this; }
};

struct CopyMove : public std::string {
    CopyMove() = default;
    CopyMove(const CopyMove &) : std::string()  { this->assign("copied-ctor"); }
    CopyMove(CopyMove &&) noexcept { this->assign("moved-ctor"); }

    CopyMove& operator=(const CopyMove &) { this->assign("copied-op"); return *this; }
    CopyMove& operator=(CopyMove &&) noexcept { this->assign("moved-op"); return *this; }
};
}

TEST_CASE("Move if possible util") {
    SECTION("Copy only type -- constructable") {
        fl_test::CopyOnly co;
        fl_test::CopyOnly co1 = fl::util::move_if_possible(co);

        REQUIRE(co1 == "copied-ctor");
    }

    SECTION("Copy only type -- assignable") {
        fl_test::CopyOnly co;
        fl_test::CopyOnly co1;
        co1 = fl::util::move_if_possible(co);

        REQUIRE(co1 == "copied-op");
    }

    SECTION("Move only type -- constructable") {
        fl_test::MoveOnly co;
        fl_test::MoveOnly co1 = fl::util::move_if_possible(co);

        REQUIRE(co1 == "moved-ctor");
    }

    SECTION("Move only type -- assignable") {
        fl_test::MoveOnly co;
        fl_test::MoveOnly co1;
        co1 = fl::util::move_if_possible(co);

        REQUIRE(co1 == "moved-op");
    }

    SECTION("Copy-move type -- constructable") {
        fl_test::CopyMove co;
        fl_test::CopyMove co1 = fl::util::move_if_possible(co);

        REQUIRE(co1 == "moved-ctor");
    }

    SECTION("Copy-move type -- assignable") {
        fl_test::CopyMove co;
        fl_test::CopyMove co1;
        co1 = fl::util::move_if_possible(co);

        REQUIRE(co1 == "moved-op");
    }
}