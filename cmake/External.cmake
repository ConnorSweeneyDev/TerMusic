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

set(VCPKG_RELEASE "2025.04.09")
set(VCPKG_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/external/vcpkg")
set(VCPKG_INSTALLED_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/external/vcpkg_installed")
if("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
  set(VCPKG_TRIPLET "x64-windows-static-release")
else()
  set(VCPKG_TRIPLET "x64-windows-static")
endif()
if(NOT EXISTS "${VCPKG_DIRECTORY}")
  execute_process(
    COMMAND cmd /C git clone https://github.com/microsoft/vcpkg.git "${VCPKG_DIRECTORY}"
  )
  execute_process(
    COMMAND cmd /C git checkout ${VCPKG_RELEASE}
    WORKING_DIRECTORY "${VCPKG_DIRECTORY}"
  )
  execute_process(COMMAND cmd /C bootstrap-vcpkg.bat WORKING_DIRECTORY "${VCPKG_DIRECTORY}")
endif()
if(NOT EXISTS "${VCPKG_INSTALLED_DIRECTORY}/installed.marker")
  execute_process(
    COMMAND cmd /C vcpkg install --triplet ${VCPKG_TRIPLET} --x-install-root "${VCPKG_INSTALLED_DIRECTORY}"
    WORKING_DIRECTORY "${VCPKG_DIRECTORY}"
  )
  execute_process(COMMAND ${CMAKE_COMMAND} -E touch "installed.marker" WORKING_DIRECTORY "${VCPKG_INSTALLED_DIRECTORY}")
endif()

list(APPEND SYSTEM_INCLUDE_DIRECTORIES "${VCPKG_INSTALLED_DIRECTORY}/${VCPKG_TRIPLET}/include")
if("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
  list(APPEND LIBRARY_DIRECTORIES "${VCPKG_INSTALLED_DIRECTORY}/${VCPKG_TRIPLET}/lib")
else()
  list(APPEND LIBRARY_DIRECTORIES "${VCPKG_INSTALLED_DIRECTORY}/${VCPKG_TRIPLET}/debug/lib")
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
