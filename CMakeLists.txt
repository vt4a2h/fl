cmake_minimum_required(VERSION 3.17)

project(
    fl
    VERSION 0.0.1
    DESCRIPTION "Cats for cpp :)"
    LANGUAGES CXX
)
  
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)
    include(CTest)
endif()

add_subdirectory(src)

set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/vcpkg.json)

if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(test)
    if (NOT ${CMAKE_BUILD_TYPE} STREQUAL "Debug")
        add_subdirectory(benchmark)
    endif()
endif()
 