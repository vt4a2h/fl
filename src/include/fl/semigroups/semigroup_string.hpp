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
#pragma once

#include <string>

#include <fl/semigroups/semigroup.hpp>

namespace fl {
template<>
struct Semigroup<std::string> {
    // TODO: write tests that cover all methods and simplify the class
//    [[nodiscard]]
//    std::string combine(const std::string& v1, const std::string& v2) const {
//        std::string result;
//
//        result.reserve(v1.size() + v2.size());
//        result.append(v1).append(v2);
//
//        return v1 + v2;
//    }

    [[nodiscard]]
    std::string combine(std::string&& v1, std::string&& v2) const {
        return std::move(v1) + std::move(v2);
    }

//    [[nodiscard]]
//    std::string combine(std::string&& v1, const std::string& v2) const {
//        return std::move(v1).append(v2);
//    }

//    [[nodiscard]]
//    std::string combine(const std::string& v1, std::string&& v2) const {
//        auto result = v1;
//        result.reserve(v1.size() + v2.size());
//        return std::move(result) + std::move(v2);
//    }
};
} // namespace fl
