add_subdirectory(lib)

add_executable(blob
    main.cpp
)
target_link_libraries(blob PRIVATE arg blob-lib)
