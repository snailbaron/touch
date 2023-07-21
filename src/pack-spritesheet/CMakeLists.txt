add_executable(pack-spritesheet
    main.cpp
)
target_link_libraries(pack-spritesheet PRIVATE
    arg
    yaml-cpp::yaml-cpp
)
