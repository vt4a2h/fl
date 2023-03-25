cmake_minimum_required(VERSION 3.17)

project(
    fl
    VERSION 1.0.0
    DESCRIPTION "Functional programming tools for C++"
    LANGUAGES CXX
)

option(FL_TEST "Build tests" OFF)
option(FL_BENCHMARK "Build benchmarks" OFF)
option(FL_DEV "Extra checks for dev builds" OFF)

if(FL_TEST OR FL_BENCHMARK)
    include(CTest)
    set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/vcpkg.json)

    enable_testing()
    add_subdirectory(test)
    if (FL_BENCHMARK)
        add_subdirectory(benchmark)
    endif()
endif()

add_subdirectory(src)
 