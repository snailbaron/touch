add_executable(asset-preview
    main.cpp
)
target_link_libraries(asset-preview PRIVATE
    tempo
    ve
    gfx
    SDL2::SDL2
)
