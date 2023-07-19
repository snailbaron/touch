function(collect_objects output_manifest output_files type)
    set(source_file_names ${ARGN})
    list(LENGTH source_file_names source_file_name_count)
    math(EXPR source_file_name_largest_index "${source_file_name_count} - 1")

    set(${output_manifest} "[]")
    set(${output_files} "")
    foreach(i RANGE ${source_file_name_largest_index})
        list(GET source_file_names ${i} source_file_name)

        set(file_path "${CMAKE_CURRENT_SOURCE_DIR}/${type}/${source_file_name}")
        if(NOT EXISTS "${file_path}")
            message(SEND_ERROR
                "resource file of type '${type}' does not exist: '${file_path}'")
        endif()

        string(JSON ${output_manifest}
            SET "${${output_manifest}}" ${i} "\"${file_path}\"")

        string(REGEX REPLACE "\\.[^\\.]*" ".png"
            processed_file_name "${source_file_name}")
        list(APPEND ${output_files} "${type}/${processed_file_name}")
    endforeach()

    return(PROPAGATE ${output_manifest} ${output_files})
endfunction()

function(resources)
    set(options "")
    set(oneValueArgs "")
    set(multiValueArgs CHARACTERS TEXTURES OBJECTS)
    cmake_parse_arguments(
        R "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    list(LENGTH R_CHARACTERS character_count)
    list(LENGTH R_TEXTURE texture_count)
    list(LENGTH R_OBJECTS object_count)

    collect_objects(character_json character_files "characters" ${R_CHARACTERS})
    collect_objects(texture_json texture_files "textures" ${R_TEXTURES})
    collect_objects(object_json object_files "objects" ${R_OBJECTS})

    set(input_manifest_path "${CMAKE_CURRENT_BINARY_DIR}/manifest.json")
    set(manifest "{}")
    string(JSON manifest SET "${manifest}" "characters" "${character_json}")
    string(JSON manifest SET "${manifest}" "textures" "${texture_json}")
    string(JSON manifest SET "${manifest}" "objects" "${object_json}")
    file(WRITE "${input_manifest_path}" "${manifest}")

    set(output_files ${character_files} ${texture_files} ${object_files})
    list(TRANSFORM output_files PREPEND "${CMAKE_CURRENT_BINARY_DIR}/sheets/")

    #add_custom_command(
    #    COMMENT "running command to pack resources"
    #    COMMAND
    #        $<TARGET_FILE:pack>
    #        --input-manifest "${input_manifest_path}"
    #        --output-manifest "${output_manifest_path}"
    #        --output-single-sheet "${CMAKE_CURRENT_BINARY_DIR}/sheet.png"
    #        --output-multiple-sheets "${CMAKE_CURRENT_BINARY_DIR}/sheets"
    #    DEPENDS
    #        pack
    #        "${input_manifest_path}"
    #    OUTPUT
    #        "${output_manifest_file}"
    #        "${CMAKE_CURRENT_BINARY_DIR}/sheet.png"
    #        ${output_files}
    #)

    #add_custom_target(pack-resources
    #    COMMENT "building target to pack resources"
    #    DEPENDS
    #        "${output_manifest_file}"
    #        "${CMAKE_CURRENT_BINARY_DIR}/sheet.png"
    #        ${output_files}
    #)
endfunction()

resources(
    CHARACTERS
        hero.aseprite
        hero-2.aseprite

    TEXTURES
        grass.aseprite
        road.aseprite
        grass-2.aseprite
        bamboo-floor.aseprite
        wooden-floor.aseprite
        wooden-walls.aseprite

    OBJECTS
        ball.aseprite
        enemy.aseprite
        tree.aseprite
        turret.aseprite
        gate.aseprite
)
