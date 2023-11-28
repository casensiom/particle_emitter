# raylib
find_package(raylib QUIET)
if (NOT raylib_FOUND)
    include(FetchContent)
    FetchContent_Declare(
        raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG 4.5.0
    )
    FetchContent_MakeAvailable(raylib)
endif()

add_library(common_dependency 
    common.h 
    common.c 
)
target_link_libraries(common_dependency PUBLIC raylib)

add_custom_target(copy_assets
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/assets ${CMAKE_CURRENT_BINARY_DIR}/assets
)
add_dependencies(common_dependency copy_assets)

add_executable(example0 example0.c)
target_link_libraries(example0 PUBLIC common_dependency)

add_executable(example1 example1.c)
target_link_libraries(example1 PUBLIC common_dependency)
