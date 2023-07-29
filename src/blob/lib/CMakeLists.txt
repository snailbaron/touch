add_custom_command(
    COMMENT "blob: generating format_generated.h"
    COMMAND
        $<TARGET_FILE:flatc>
        -o include/blob
        --cpp
        --scoped-enums
        --no-emit-min-max-enum-values
        ${CMAKE_CURRENT_SOURCE_DIR}/format.fbs
    DEPENDS format.fbs
    OUTPUT include/blob/format_generated.h
)

add_library(blob-lib
    blob.cpp
    header.cpp
    pack.cpp
    types.cpp

    format.fbs
    ${CMAKE_CURRENT_BINARY_DIR}/include/blob/format_generated.h
)
target_include_directories(blob-lib PUBLIC
    include
    ${CMAKE_CURRENT_BINARY_DIR}/include
)
target_link_libraries(blob-lib PRIVATE fi flatbuffers)
