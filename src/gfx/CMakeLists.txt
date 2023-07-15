add_library(gfx
    sdl-wrapper.cpp
)
target_include_directories(gfx PUBLIC include)
target_link_libraries(gfx PRIVATE SDL2::SDL2)
