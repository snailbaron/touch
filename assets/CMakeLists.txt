function(add_pack_spritesheet_target)
    set(options "")
    set(oneValueArgs TARGET MANIFEST OUTPUT_SHEET OUTPUT_META)
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
        DEPENDS
            pack-spritesheet
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

add_pack_spritesheet_target(
    TARGET pack-spritesheet-target
    MANIFEST manifest.yaml
    OUTPUT_SHEET sheet.png
    OUTPUT_META sheet-meta.yaml

    FILES
        character characters/hero.aseprite
        character characters/hero-2.aseprite

        texture textures/grass.aseprite
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
