include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/Manager.cmake")
set(SQLITE_VERSION "3490100")
set(SDL_VERSION "2.32.4")
set(SDL_MIXER_VERSION "2.8.1")
set(FTXUI_VERSION "6.0.2")

CPMAddPackage(NAME "sqlite" URL "https://www.sqlite.org/2025/sqlite-amalgamation-${SQLITE_VERSION}.zip" DOWNLOAD_ONLY)
list(APPEND SYSTEM_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_BINARY_DIR}/_deps/sqlite-src")
list(APPEND SOURCE_FILES "${CMAKE_CURRENT_BINARY_DIR}/_deps/sqlite-src/sqlite3.c")

CPMAddPackage(
  URI
  "gh:libsdl-org/SDL#release-${SDL_VERSION}"
  OPTIONS
    "SDL_TEST_ENABLED_BY_DEFAULT OFF"
    "SDL_TEST OFF"
    "SDL2_DISABLE_INSTALL ON"
    "SDL2_DISABLE_UNINSTALL ON"
    "SDL2_DISABLE_SDL2MAIN ON"
)
CPMAddPackage(
  URI
  "gh:libsdl-org/SDL_mixer#release-${SDL_MIXER_VERSION}"
  OPTIONS
    "SDL2MIXER_OPUS OFF"
    "SDL2MIXER_WAVPACK OFF"
    "SDL2MIXER_MOD OFF"
    "SDL2MIXER_FLAC OFF"
    "SDL2MIXER_MIDI OFF"
    "SDL2MIXER_GME OFF"
    "SDL2MIXER_OGG OFF"
    "SDL2MIXER_VORBIS OFF"
)
list(
  APPEND
  SYSTEM_INCLUDE_DIRECTORIES
  "${CMAKE_CURRENT_BINARY_DIR}/_deps/sdl-build/include/SDL2"
  "${CMAKE_CURRENT_BINARY_DIR}/_deps/sdl-build/include-config-${CMAKE_BUILD_TYPE_LOWER}/SDL2"
  "${CMAKE_CURRENT_BINARY_DIR}/_deps/sdl_mixer-src/include"
)
list(APPEND COMPILER_DEFINITIONS "SDL_MAIN_HANDLED")
list(APPEND LIBRARIES "SDL2::SDL2-static" "SDL2_mixer::SDL2_mixer-static")

CPMAddPackage(URI "gh:ArthurSonzogni/FTXUI#v${FTXUI_VERSION}" OPTIONS "FTXUI_ENABLE_INSTALL OFF")
list(APPEND SYSTEM_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_BINARY_DIR}/_deps/ftxui-src/include")
list(APPEND LIBRARIES "ftxui::component" "ftxui::dom" "ftxui::screen")

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
