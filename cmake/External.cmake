list(APPEND SYSTEM_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/external/sqlite/include")
list(APPEND SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/external/sqlite/source/sqlite3.c")

set(SDL_TEST_ENABLED_BY_DEFAULT OFF CACHE BOOL "")
set(SDL_TEST OFF CACHE BOOL "")
set(SDL2_DISABLE_INSTALL ON CACHE BOOL "")
set(SDL2_DISABLE_UNINSTALL ON CACHE BOOL "")
set(SDL2_DISABLE_SDL2MAIN ON CACHE BOOL "")
set(SDL2MIXER_OPUS OFF CACHE BOOL "")
set(SDL2MIXER_WAVPACK OFF CACHE BOOL "")
set(SDL2MIXER_MOD OFF CACHE BOOL "")
set(SDL2MIXER_FLAC OFF CACHE BOOL "")
set(SDL2MIXER_MIDI OFF CACHE BOOL "")
set(SDL2MIXER_GME OFF CACHE BOOL "")
set(SDL2MIXER_OGG OFF CACHE BOOL "")
set(SDL2MIXER_VORBIS "" CACHE STRING "")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/external/SDL2" EXCLUDE_FROM_ALL)
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/external/SDL_mixer" EXCLUDE_FROM_ALL)
list(
  APPEND
  SYSTEM_INCLUDE_DIRECTORIES
  "${CMAKE_CURRENT_BINARY_DIR}/external/SDL2/include"
  "${CMAKE_CURRENT_BINARY_DIR}/external/SDL2/include-config-${CMAKE_BUILD_TYPE_LOWER}"
  "${CMAKE_CURRENT_BINARY_DIR}/external/SDL2/include-config-${CMAKE_BUILD_TYPE_LOWER}/SDL2"
)
list(APPEND COMPILER_DEFINITIONS "SDL_MAIN_HANDLED")
list(APPEND LIBRARIES "SDL2::SDL2-static" "SDL2_mixer::SDL2_mixer-static")

set(FTXUI_ENABLE_INSTALL OFF CACHE BOOL "")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/external/ftxui" EXCLUDE_FROM_ALL)
list(APPEND SYSTEM_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/external/ftxui/include")
list(APPEND LIBRARIES "ftxui::component" "ftxui::dom" "ftxui::screen")

set(VCPKG_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/external/vcpkg")
set(VCPKG_INSTALLED_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg_installed")
add_custom_command(
  OUTPUT "${VCPKG_DIRECTORY}/vcpkg.exe"
  COMMAND
    if not exist "${VCPKG_DIRECTORY}/.git"
    (if exist "${VCPKG_DIRECTORY}" (${CMAKE_COMMAND} -E remove_directory "${VCPKG_DIRECTORY}"))
  COMMAND if not exist "${VCPKG_DIRECTORY}" (git clone https://github.com/microsoft/vcpkg.git "${VCPKG_DIRECTORY}")
  COMMAND ${CMAKE_COMMAND} -E chdir "${VCPKG_DIRECTORY}" cmd /C bootstrap-vcpkg.bat
)
add_custom_target("vcpkg" DEPENDS "${VCPKG_DIRECTORY}/vcpkg.exe")
list(APPEND DEPENDENCIES "vcpkg")

add_custom_command(
  OUTPUT "${VCPKG_INSTALLED_DIRECTORY}/x64-windows-static/share/ffmpeg/FindFFMPEG.cmake"
  COMMAND cmd /C "${VCPKG_DIRECTORY}/vcpkg.exe" install --triplet x64-windows-static
  DEPENDS "${VCPKG_DIRECTORY}/vcpkg.exe"
)
add_custom_target("ffmpeg" DEPENDS "${VCPKG_INSTALLED_DIRECTORY}/x64-windows-static/share/ffmpeg/FindFFMPEG.cmake")
list(APPEND DEPENDENCIES "ffmpeg")
list(APPEND SYSTEM_INCLUDE_DIRECTORIES "${VCPKG_DIRECTORY}/packages/ffmpeg_x64-windows-static/include")
if("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
  list(APPEND LIBRARY_DIRECTORIES "${VCPKG_DIRECTORY}/packages/ffmpeg_x64-windows-static/lib")
else()
  list(APPEND LIBRARY_DIRECTORIES "${VCPKG_DIRECTORY}/packages/ffmpeg_x64-windows-static/debug/lib")
endif()
list(
  APPEND
  LIBRARIES
  "avcodec"
  "avdevice"
  "avfilter"
  "avformat"
  "avutil"
  "swresample"
  "swscale"
  "dxva2"
  "evr"
  "mf"
  "mfplat"
  "mfplay"
  "mfreadwrite"
  "mfuuid"
  "bcrypt"
  "ws2_32"
  "Secur32"
  "Strmiids"
)
