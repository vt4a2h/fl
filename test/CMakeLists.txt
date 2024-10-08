include(${CMAKE_SOURCE_DIR}/cmake/helpers.cmake)

has_expected(HAS_EXPECTED_RESULT)
message(STATUS "Has std::expected support: ${HAS_EXPECTED_RESULT}")

find_package(Catch2 CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)

add_executable(tests)

target_sources(tests PRIVATE
    test_writer_create.cpp
    test_writer_transform.cpp
    test_writer_and_then.cpp
    test_writer_tell.cpp
    test_writer_other_methods.cpp
    test_writer_custom_semigroup.cpp
    test_writer_any_semigroup.cpp
    test_writer_ap.cpp
    test_semigroups.cpp
    test_monoids.cpp
    test_factorial_writer.cpp
    test_writer_non_movable.cpp
    test_writer_non_copyable.cpp
    test_lazy_syntax.cpp
    writer_default_types.hpp
    writer_utility_types.hpp
    test_writer_move_copy.cpp
    test_utils_ap_optional.cpp
    test_applicative_optional.cpp
    test_expected_create.cpp
    test_expected_unexpected_create.cpp
    test_expected_unexpected_access_error.cpp
)

if (HAS_EXPECTED_RESULT)
    target_sources(tests PRIVATE
        test_utils_ap_expected.cpp
    )
endif ()

target_compile_features(tests PRIVATE cxx_std_23)

target_link_libraries(tests PRIVATE
    Catch2::Catch2
    Catch2::Catch2WithMain
    fmt::fmt
    fl
)

include(Catch)
catch_discover_tests(tests)
