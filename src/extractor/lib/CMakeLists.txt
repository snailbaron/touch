add_library(extractor
    extractor.cpp
)
target_include_directories(extractor PUBLIC include)
target_link_libraries(extractor PRIVATE nlohmann_json::nlohmann_json)
