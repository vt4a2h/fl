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

#include <memory>
#include <filesystem>

#include <spdlog/logger.h>

namespace common::util {

class LoggingFixture
{
public:
    using SharedLogger = std::shared_ptr<spdlog::logger>;

    LoggingFixture();
    ~LoggingFixture();

protected:
    [[nodiscard]] const SharedLogger &logger() const;

private:
    std::pair<std::filesystem::path, SharedLogger> logger_;
};

} // namespace common::util
