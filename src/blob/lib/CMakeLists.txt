add_custom_command(
    COMMENT "blob: generating format_generated.h"
    COMMAND
        $<TARGET_FILE:flatc>
        -o include
        --cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/format.fbs
    DEPENDS format.fbs
    OUTPUT include/format_generated.h
)

add_library(blob-lib
    pack.cpp
    unpacked.cpp

    format.fbs
    ${CMAKE_CURRENT_BINARY_DIR}/include/format_generated.h
)
target_include_directories(blob-lib
    PUBLIC include
    PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/include
)
target_link_libraries(blob-lib PRIVATE fi)
