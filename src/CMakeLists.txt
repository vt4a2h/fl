add_library(fl INTERFACE)

add_library(fl::fl ALIAS fl)

if (FL_DEV)
    set_target_properties(fl PROPERTIES
        CXX_STANDARD 23
        CMAKE_CXX_EXTENSIONS OFF
    )

    if(NOT MSVC)
        target_compile_options(fl INTERFACE -Wall -Wextra -pedantic -Werror)
    else ()
        target_compile_options(fl INTERFACE /W3 /WX)
    endif()
endif()

target_include_directories(
    fl INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

include(CMakePackageConfigHelpers)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/fl-config-version.cmake
    VERSION ${CMAKE_PROJECT_VERSION}
    COMPATIBILITY ExactVersion
)

include(GNUInstallDirs)

install(
    TARGETS fl
    EXPORT fl-targets
    DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
install(
    EXPORT fl-targets FILE fl-targets.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake
)
install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/fl-config-version.cmake
        ${fl_SOURCE_DIR}/cmake/fl-config.cmake
    DESTINATION
        ${CMAKE_INSTALL_LIBDIR}/cmake
)
install(
    DIRECTORY include/fl
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*"
)
export(EXPORT fl-targets FILE fl-config.cmake)
