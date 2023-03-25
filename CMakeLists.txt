cmake_minimum_required(VERSION 3.17)

project(
    fl
    VERSION 1.0.0
    DESCRIPTION "Functional programming tools for C++"
    LANGUAGES CXX
)
  
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)
    include(CTest)
endif()

add_subdirectory(src)

if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(test)
    if (NOT ${CMAKE_BUILD_TYPE} STREQUAL "Debug")
        add_subdirectory(benchmark)
    endif()
endif()
 