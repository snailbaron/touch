function(pack_spritesheet)
    set(options "")
    set(oneValueArgs TARGET MANIFEST OUTPUT_SHEET OUTPUT_META TOUCH)
    set(multiValueArgs FILES)
    cmake_parse_arguments(
        ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    set(manifest_file_path "${CMAKE_CURRENT_BINARY_DIR}/${ARG_MANIFEST}")
    set(output_sheet_file_path "${CMAKE_CURRENT_BINARY_DIR}/${ARG_OUTPUT_SHEET}")
    set(output_meta_file_path "${CMAKE_CURRENT_BINARY_DIR}/${ARG_OUTPUT_META}")

    file(WRITE "${manifest_file_path}" "")

    set(input_file_paths "")
    while(NOT "${ARG_FILES}" STREQUAL "")
        list(POP_FRONT ARG_FILES type path)
        list(APPEND input_file_paths "${path}")
        file(APPEND "${manifest_file_path}" "- type: ${type}\n  path: \"${path}\"\n")
    endwhile()

    add_custom_command(
        COMMENT "packing spritesheet"
        COMMAND
            $<TARGET_FILE:pack-spritesheet>
            --aseprite "$<TARGET_FILE:aseprite>"
            --root "${CMAKE_CURRENT_SOURCE_DIR}"
            --manifest "${manifest_file_path}"
            --output-sheet "${output_sheet_file_path}"
            --output-meta "${output_meta_file_path}"
        COMMAND "${CMAKE_COMMAND}" -E
            touch "${CMAKE_CURRENT_BINARY_DIR}/${ARG_TOUCH}"
        DEPENDS
            pack-spritesheet
            aseprite
            "${manifest_file_path}"
            ${input_file_paths}
        OUTPUT
            "${output_sheet_file_path}"
            "${output_meta_file_path}"
    )

    add_custom_target(${ARG_TARGET}
        COMMENT "running target to pack spritesheet"
        DEPENDS
            "${output_sheet_file_path}"
            "${output_meta_file_path}"
    )
endfunction()

function(pack_music)
    set(options "")
    set(oneValueArgs TARGET OUTPUT_DIRECTORY TOUCH)
    set(multiValueArgs FILES)
    cmake_parse_arguments(
        ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    set(output_file_paths "")
    foreach(relative_input_file_path ${ARG_FILES})
        cmake_path(APPEND input_file_path
            "${CMAKE_CURRENT_SOURCE_DIR}"
            "${relative_input_file_path}")
        cmake_path(GET input_file_path STEM LAST_ONLY input_file_stem)
        cmake_path(APPEND output_file_path
            "${CMAKE_CURRENT_BINARY_DIR}"
            "${ARG_OUTPUT_DIRECTORY}"
            "${input_file_stem}.wav")

        add_custom_command(
            COMMENT "packing music"
            COMMAND
                "$<TARGET_FILE:lmms>"
                render "${input_file_path}"
                --loop
                --output "${output_file_path}"
            COMMAND "${CMAKE_COMMAND}" -E
                touch "${CMAKE_CURRENT_BINARY_DIR}/${ARG_TOUCH}"
            DEPENDS
                lmms
                "${input_file_path}"
            OUTPUT
                "${output_file_path}"
        )
        list(APPEND output_file_paths "${output_file_path}")
    endforeach()

    add_custom_target("${ARG_TARGET}"
        COMMENT "running target to pack music"
        DEPENDS
            ${output_file_paths}
    )
endfunction()

function(pack_levels)
    set(options "")
    set(oneValueArgs TARGET OUTPUT_DIRECTORY TOUCH)
    set(multiValueArgs LEVELS TILESETS)
    cmake_parse_arguments(
        ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    set(output_file_paths "")

    foreach(level_file_relative_path ${ARG_LEVELS})
        cmake_path(APPEND level_file_path
            "${CMAKE_CURRENT_SOURCE_DIR}" "${level_file_relative_path}")
        cmake_path(GET level_file_path STEM LAST_ONLY stem)
        cmake_path(APPEND output_file_path
            "${CMAKE_CURRENT_BINARY_DIR}"
            "${ARG_OUTPUT_DIRECTORY}"
            "${stem}.json")

        add_custom_command(
            COMMENT "packing level ${level_file_relative_path}"
            COMMAND
                "$<TARGET_FILE:tiled>"
                --export-map "${level_file_path}" "${output_file_path}"
            COMMAND "${CMAKE_COMMAND}" -E
                touch "${CMAKE_CURRENT_BINARY_DIR}/${ARG_TOUCH}"
            DEPENDS
                tiled
                "${level_file_path}"
            OUTPUT
                "${output_file_path}"
        )

        list(APPEND output_file_paths "${output_file_path}")
        list(APPEND  "${output_file_path}")
    endforeach()

    foreach(tileset_file_relative_path ${ARG_TILESETS})
        cmake_path(APPEND tileset_file_path
            "${CMAKE_CURRENT_SOURCE_DIR}" "${tileset_file_relative_path}")
        cmake_path(GET tileset_file_path STEM LAST_ONLY stem)
        cmake_path(APPEND output_file_path
            "${CMAKE_CURRENT_BINARY_DIR}"
            "${ARG_OUTPUT_DIRECTORY}"
            "${stem}.json")

        add_custom_command(
            COMMENT "packing tileset ${tileset_file_relative_path}"
            COMMAND
                "$<TARGET_FILE:tiled>"
                --export-tileset "${tileset_file_path}" "${output_file_path}"
            COMMAND "${CMAKE_COMMAND}" -E
                touch "${CMAKE_CURRENT_BINARY_DIR}/${ARG_TOUCH}"
            DEPENDS
                tiled
                "${tileset_file_path}"
            OUTPUT
                "${output_file_path}"
        )

        list(APPEND output_file_paths "${output_file_path}")
    endforeach()

    add_custom_target("${ARG_TARGET}"
        COMMENT "running target to pack levels"
        DEPENDS
            ${output_file_paths})
endfunction()

pack_spritesheet(
    TARGET pack-spritesheet-target
    MANIFEST manifest.yaml
    OUTPUT_SHEET extracted/sheet/sheet.png
    OUTPUT_META extracted/sheet/sheet-meta.yaml
    TOUCH timestamp

    FILES
        character characters/hero.aseprite
        character characters/hero-2.aseprite

        texture textures/road.aseprite
        texture textures/grass-2.aseprite
        texture textures/bamboo-floor.aseprite
        texture textures/wooden-floor.aseprite
        texture textures/wooden-walls.aseprite

        object objects/ball.aseprite
        object objects/enemy.aseprite
        object objects/tree.aseprite
        object objects/turret.aseprite
        object objects/gate.aseprite
)

pack_music(
    TARGET pack-music-target
    OUTPUT_DIRECTORY extracted/music
    TOUCH timestamp
    FILES
        music/happy-start.mmpz
        music/slow.mmpz
)

pack_levels(
    TARGET pack-levels-target
    OUTPUT_DIRECTORY extracted/levels
    TOUCH timestamp

    LEVELS
        levels/courtyard.tmx
        levels/room.tmx
    TILESETS
        levels/sheet.tsx
)

file(CREATE_LINK
    "${CMAKE_CURRENT_BINARY_DIR}/extracted"
    "${PROJECT_SOURCE_DIR}/extracted"
    SYMBOLIC
)
