cmake_minimum_required(VERSION 3.17)

project(
    fl
    VERSION 0.1
    DESCRIPTION "Cats for cpp :)"
    LANGUAGES CXX
)
  
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    set(CXX_STANDARD 23)
    set(CMAKE_CXX_EXTENSIONS OFF)
    if(NOT MSVC)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Werror")
    endif()

    set_property(GLOBAL PROPERTY USE_FOLDERS ON)
    
    include(CTest)
endif()

include_directories(${CMAKE_SOURCE_DIR}/include)
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/vcpkg.json)

if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(test)
    if (NOT ${CMAKE_BUILD_TYPE} STREQUAL "Debug")
        add_subdirectory(benchmark)
    endif()
endif()
 