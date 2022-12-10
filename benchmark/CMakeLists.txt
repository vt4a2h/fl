find_package(Catch2 CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)

set(HEADER_LIST
    common/util.hpp
)
set(SOURCE_LIST

    benchmark_factorial_writer.cpp
    benchmark_sum.cpp

    common/util.cpp

    common/logging_fixture.cpp)

add_executable(benchmarks
    main.cpp
    ${HEADER_LIST}
    ${SOURCE_LIST}
)

target_compile_features(benchmarks PRIVATE cxx_std_23)

target_link_libraries(benchmarks PRIVATE
    Catch2::Catch2
    Catch2::Catch2WithMain
    fmt::fmt
    spdlog::spdlog
)

include(Catch)
catch_discover_tests(benchmarks)
