find_package(Catch2 CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)

add_executable(benchmarks)

target_sources(benchmarks PRIVATE
    main.cpp
    benchmark_factorial_writer.cpp
    benchmark_sum.cpp

    common/util.cpp

    common/logging_fixture.cpp

    common/util.hpp
)

target_compile_features(benchmarks PRIVATE cxx_std_23)

target_link_libraries(benchmarks PRIVATE
    Catch2::Catch2
    Catch2::Catch2WithMain
    fmt::fmt
    spdlog::spdlog
    fl
)

if(MSVC)
    target_compile_options(benchmarks
            PRIVATE
            "/wd4996;" # external warning in fmt
    )
endif()

include(Catch)
catch_discover_tests(benchmarks)
