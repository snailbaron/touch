set(ENABLE_NEWS OFF)
set(ENABLE_UPDATER OFF)
set(ENABLE_SCRIPTING OFF)
set(ENABLE_WEBSOCKET OFF)
set(ENABLE_UI OFF)
set(ENABLE_CCACHE OFF)
set(ENABLE_WEBP OFF)

set(SDL2IMAGE_INSTALL OFF)

add_subdirectory(Catch2)
add_subdirectory(SDL)
add_subdirectory(SDL_image)
add_subdirectory(aseprite)
add_subdirectory(flatbuffers)
add_subdirectory(json)
add_subdirectory(yaml-cpp)

include(ExternalProject)

ExternalProject_Add(
    lmms-project
    GIT_REPOSITORY https://github.com/LMMS/lmms.git
    GIT_TAG 94363be152f526edba4e884264d891f1361cf54b
    GIT_SHALLOW ON
    UPDATE_COMMAND ""
    CMAKE_ARGS
        -D WANT_QT5=ON
        -D "CMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}/lmms"
)
add_executable(lmms IMPORTED GLOBAL)
set_target_properties(lmms PROPERTIES
    IMPORTED_LOCATION "${CMAKE_CURRENT_BINARY_DIR}/lmms/bin/lmms"
)

ExternalProject_Add(
    tiled-project
    GIT_REPOSITORY https://github.com/mapeditor/tiled.git
    GIT_TAG 3b2a3c47cb1c8affe59865f671663b7c6ca2e8ef
    GIT_SHALLOW ON
    UPDATE_COMMAND ""
    BUILD_IN_SOURCE ON
    CONFIGURE_COMMAND qbs setup-toolchains --detect
        COMMAND qbs setup-qt --detect
    BUILD_COMMAND qbs
    INSTALL_COMMAND
        qbs install --install-root "${CMAKE_CURRENT_BINARY_DIR}/tiled"
)
add_executable(tiled IMPORTED GLOBAL)
set_target_properties(tiled PROPERTIES
    IMPORTED_LOCATION "${CMAKE_CURRENT_BINARY_DIR}/tiled/usr/local/bin/tiled")

