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
#include "util.hpp"

#include <random>

#include <spdlog/sinks/rotating_file_sink.h>

#include <range/v3/all.hpp>

#include <fmt/format.h>

static std::string randomSuffix(std::size_t charsCount = 32) {
    namespace r = ranges;
    namespace v = r::views;

    static auto chars = [] {
        const auto digits = v::ints(48, 58);
        const auto lowerCaseLetters = v::ints(97, 123);

        return r::to<std::string>(v::concat(digits, lowerCaseLetters) | v::transform([](int i) { return char(i); }));
    }();

    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<std::size_t> dis(std::size_t{}, chars.size() - 1);

    return r::accumulate(v::ints(std::size_t{}, charsCount) | v::transform([&](auto) { return chars.at(dis(gen)); }),
                         std::string{});
}

namespace common::util {

std::filesystem::path defaultLogsPath() {
    return std::filesystem::temp_directory_path() / "fl_benchmark_logs";
}

std::pair<std::filesystem::path, std::shared_ptr<spdlog::logger>> file_logger()
{
    const auto suffix = randomSuffix();
    const std::size_t max_size_bytes = 1 << (10 * 3);
    const std::size_t max_files_count = 1;

    auto path = (defaultLogsPath() / fmt::format("benchmark_{}.log", suffix)).string();
    auto logger = spdlog::rotating_logger_st(fmt::format("benchmark_logger_{}", suffix), path, max_size_bytes, max_files_count);

    return std::make_pair(std::move(path), std::move(logger));
}

} // namespace common::util
