add_library(fl INTERFACE)

set_target_properties(fl PROPERTIES
    CXX_STANDARD 23
    CMAKE_CXX_EXTENSIONS OFF
)

if(NOT MSVC)
    target_compile_options(fl INTERFACE -Wall -Wextra -pedantic -Werror)
endif()

target_include_directories(fl INTERFACE include)

install(DIRECTORY include DESTINATION include)
