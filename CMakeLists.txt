cmake_minimum_required(VERSION 3.25)

project(Touch)

set(CMAKE_COLOR_DIAGNOSTICS ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

add_subdirectory(deps)

if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(
        -Wall -Wextra -pedantic -Werror -Wno-missing-field-initializers)
endif()

add_subdirectory(src/arg)
add_subdirectory(src/as)
add_subdirectory(src/evening)
add_subdirectory(src/fi)
add_subdirectory(src/tempo)
add_subdirectory(src/thing)
add_subdirectory(src/ve)

# add_subdirectory(src/extractor)
add_subdirectory(assets)

# add_subdirectory(src/blob)
# add_subdirectory(src/gfx)
# add_subdirectory(src/touch)

add_subdirectory(src/pack-spritesheet)
