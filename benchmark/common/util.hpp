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

#include <type_traits>
#include <functional>
#include <filesystem>

#include <spdlog/logger.h>

namespace common::util {

std::filesystem::path defaultLogsPath();

std::pair<std::filesystem::path, std::shared_ptr<spdlog::logger>> file_logger();

} // namespace common::util