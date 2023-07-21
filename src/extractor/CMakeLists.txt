add_subdirectory(lib)

add_executable(extract
    main.cpp
)
target_link_libraries(extract PRIVATE
    arg
    extractor
    nlohmann_json::nlohmann_json
)
