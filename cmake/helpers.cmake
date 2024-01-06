function(has_expected OUT_RESULT)
    try_compile(COMPILE_RESULT
        ${CMAKE_BINARY_DIR}/test_apps
        ${CMAKE_SOURCE_DIR}/cmake/try-compile-apps/expected.cpp
        CXX_STANDARD 23
        CXX_STANDARD_REQUIRED TRUE
        CXX_EXTENSIONS FALSE
    )
    set(${OUT_RESULT} ${COMPILE_RESULT} PARENT_SCOPE)
endfunction()