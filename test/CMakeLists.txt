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
    test_functor.cpp
    test_factorial_writer.cpp
    test_writer_non_movable.cpp
    test_writer_non_copyable.cpp
    test_move_if_possible.cpp
    test_lazy_syntax.cpp
    writer_default_types.hpp
    test_writer_move_copy.cpp
)

target_compile_features(tests PRIVATE cxx_std_23)

target_link_libraries(tests PRIVATE
    Catch2::Catch2
    Catch2::Catch2WithMain
    fmt::fmt
    fl
)

include(Catch)
catch_discover_tests(tests)
