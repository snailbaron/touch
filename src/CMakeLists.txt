if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

add_subdirectory(arg)
add_subdirectory(as)
add_subdirectory(evening)
add_subdirectory(fi)
add_subdirectory(tempo)
add_subdirectory(thing)
add_subdirectory(ve)

add_subdirectory(gfx)
add_subdirectory(blob)

add_subdirectory(touch)
