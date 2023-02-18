include("${CMAKE_CURRENT_LIST_DIR}/fl-targets.cmake")
add_library(fl::fl INTERFACE IMPORTED)
target_link_libraries(fl::fl INTERFACE fl)
