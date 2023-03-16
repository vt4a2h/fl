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
#include "logging_fixture.hpp"

#include <iostream>

#include "util.hpp"

#include "spdlog/spdlog.h"

#include <fmt/format.h>

namespace common::util {

LoggingFixture::LoggingFixture()
    : logger_(file_logger())
{
}

LoggingFixture::~LoggingFixture()
{
    spdlog::drop(logger_.second->name());
    logger_.second.reset();

    try {
        std::filesystem::remove_all(logger_.first.parent_path());
    } catch (const std::exception &e) {
        fmt::print("[LoggingFixture] Error: {}", e.what());
    }
}

const LoggingFixture::SharedLogger &common::util::LoggingFixture::logger() const
{
    return logger_.second;
}

} // namespace common::util
