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

#include <vector>
#include <string>

#include <fl/writer.hpp>
#include <fl/semigroups/semigroup_std_container.hpp>

using Log = std::vector<std::string>;
using Value = std::uint64_t;
using Logger = fl::Writer<Log, Value>;
