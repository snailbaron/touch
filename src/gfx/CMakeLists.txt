add_custom_command(
    COMMENT "generating resource_ids.h"
    COMMAND
        $<TARGET_FILE:blob>
        --aseprite $<TARGET_FILE:aseprite>
        --input ${PROJECT_SOURCE_DIR}/assets
        --output-data ${PROJECT_BINARY_DIR}/data.blob
        --output-header include/gfx/resource_ids.hpp
        --action pack
    OUTPUT include/gfx/resource_ids.hpp
)

add_library(gfx
    resources.cpp
    scene.cpp
    sdl-wrapper.cpp
    view.cpp

    ${CMAKE_CURRENT_BINARY_DIR}/include/gfx/resource_ids.hpp
)
target_include_directories(gfx PUBLIC
    include
    ${CMAKE_CURRENT_BINARY_DIR}/include
)
target_link_libraries(gfx
    PUBLIC
        blob-lib

        fi
        tempo
        ve

        SDL2::SDL2
        SDL2_image::SDL2_image
)
