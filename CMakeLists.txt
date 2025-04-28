cmake_minimum_required(VERSION 4.0)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
string(TOLOWER "${CMAKE_BUILD_TYPE}" CMAKE_BUILD_TYPE_LOWER)
if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_CURRENT_SOURCE_DIR}")
  set(IS_SUBDIRECTORY OFF)
else()
  set(IS_SUBDIRECTORY ON)
endif()

set(OUTPUT "TerMusic")
project(${OUTPUT} VERSION 0.0.0)
set(INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/program/include")
set(
  SYSTEM_INCLUDE_DIRS
  "${CMAKE_CURRENT_BINARY_DIR}/external/SDL2/include"
  "${CMAKE_CURRENT_BINARY_DIR}/external/SDL2/include-config-${CMAKE_BUILD_TYPE_LOWER}"
  "${CMAKE_CURRENT_BINARY_DIR}/external/SDL2/include-config-${CMAKE_BUILD_TYPE_LOWER}/SDL2"
  "${CMAKE_CURRENT_SOURCE_DIR}/external/ftxui/include"
  "${CMAKE_CURRENT_SOURCE_DIR}/external/taglib/include"
  "${CMAKE_CURRENT_SOURCE_DIR}/external/sqlite/include"
)
file(GLOB HEADER_FILES "${CMAKE_CURRENT_SOURCE_DIR}/program/include/*.*")
file(
  GLOB SOURCE_FILES
  "${CMAKE_CURRENT_SOURCE_DIR}/program/source/*.*"
  "${CMAKE_CURRENT_SOURCE_DIR}/external/sqlite/source/*.*"
)
source_group("Header Files" FILES "${HEADER_FILES}")
source_group("Source Files" FILES "${SOURCE_FILES}")
set(CMAKE_CXX_STANDARD 20)
set(COMPILER_WARNINGS "/W4")
set(BUILD_SHARED_LIBS OFF)
set(FORCE_STATIC_VCRT ON)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
add_link_options("$<$<CONFIG:Release>:/INCREMENTAL:NO>")

add_subdirectory("external/SDL2")
add_subdirectory("external/SDL_mixer")
list(APPEND LIBRARIES "SDL2::SDL2-static" "SDL2_mixer::SDL2_mixer-static")
list(APPEND COMPILER_DEFINITIONS "SDL_MAIN_HANDLED")

add_subdirectory("external/ftxui")
list(APPEND LIBRARIES "ftxui::component" "ftxui::dom" "ftxui::screen")

add_subdirectory("external/taglib")
list(APPEND LIBRARIES "tag")
list(APPEND COMPILER_DEFINITIONS "TAGLIB_STATIC")

add_executable("${OUTPUT}")
target_include_directories("${OUTPUT}" PRIVATE "${INCLUDE_DIRS}")
target_include_directories("${OUTPUT}" SYSTEM PRIVATE "${SYSTEM_INCLUDE_DIRS}")
target_sources("${OUTPUT}" PRIVATE "${HEADER_FILES}" "${SOURCE_FILES}")
target_compile_options("${OUTPUT}" PRIVATE "${COMPILER_WARNINGS}")
target_compile_definitions("${OUTPUT}" PRIVATE "${COMPILER_DEFINITIONS}")
target_link_libraries("${OUTPUT}" PRIVATE "${LIBRARIES}")

if(NOT "${IS_SUBDIRECTORY}")
  set(FORMAT_MARKER_DIR "${CMAKE_CURRENT_BINARY_DIR}/FormatMarkers")
  set(FORMAT_MARKERS "")
  file(GLOB FORMAT_FILES "${CMAKE_CURRENT_SOURCE_DIR}/program/**/*.*")
  foreach(FILE ${FORMAT_FILES})
    get_filename_component(FILE_NAME "${FILE}" NAME)
    set(FORMAT_FILE "${FORMAT_MARKER_DIR}/${FILE_NAME}.format")
    add_custom_command(
      OUTPUT "${FORMAT_FILE}"
      COMMAND clang-format -i "${FILE}"
      COMMAND "${CMAKE_COMMAND}" -E make_directory "${FORMAT_MARKER_DIR}"
      COMMAND "${CMAKE_COMMAND}" -E touch "${FORMAT_FILE}"
      DEPENDS "${FILE}"
    )
    list(APPEND FORMAT_MARKERS "${FORMAT_FILE}")
  endforeach()
  add_custom_target("Format" DEPENDS "${FORMAT_MARKERS}")
  add_dependencies("${OUTPUT}" "Format")

  set(COMPILE_COMMANDS_FILE "${CMAKE_CURRENT_SOURCE_DIR}/compile_commands.json")
  file(WRITE "${COMPILE_COMMANDS_FILE}" "[\n")
  foreach(FILE ${SOURCE_FILES})
    list(LENGTH SOURCE_FILES NUM_SOURCE_FILES)
    get_filename_component(FILE_EXT "${FILE}" EXT)
    if("${FILE_EXT}" MATCHES "cpp")
      file(
        APPEND
        "${COMPILE_COMMANDS_FILE}"
        "  {\n    \"directory\": \"${CMAKE_CURRENT_SOURCE_DIR}\",\n    \"file\": \"${FILE}\",\n    \"command\": \"clang++ -std=c++${CMAKE_CXX_STANDARD} -Wall -Wextra -Wpedantic -Wconversion -Wshadow-all -Wundef -Wdeprecated -Wtype-limits -Wcast-qual -Wcast-align -Wfloat-equal -Wparentheses -Wunreachable-code-aggressive -Wformat=2 "
      )
    else()
      file(
        APPEND
        "${COMPILE_COMMANDS_FILE}"
        "  {\n    \"directory\": \"${CMAKE_CURRENT_SOURCE_DIR}\",\n    \"file\": \"${FILE}\",\n    \"command\": \"clang "
      )
    endif()
    foreach(DIR ${INCLUDE_DIRS})
      file(APPEND "${COMPILE_COMMANDS_FILE}" "-I\\\"${DIR}\\\" ")
    endforeach()
    foreach(DIR ${SYSTEM_INCLUDE_DIRS})
      file(APPEND "${COMPILE_COMMANDS_FILE}" "-isystem\\\"${DIR}\\\" ")
    endforeach()
    foreach(DEFINITION ${COMPILER_DEFINITIONS})
      file(APPEND "${COMPILE_COMMANDS_FILE}" "-D${DEFINITION} ")
    endforeach()
    file(APPEND "${COMPILE_COMMANDS_FILE}" "-c \\\"${FILE}\\\"\"\n  }")
    math(EXPR COMPILE_COMMANDS_INDEX "${COMPILE_COMMANDS_INDEX} + 1")
    if(NOT "${COMPILE_COMMANDS_INDEX}" EQUAL "${NUM_SOURCE_FILES}")
      file(APPEND "${COMPILE_COMMANDS_FILE}" ",\n")
    else()
      file(APPEND "${COMPILE_COMMANDS_FILE}" "\n]")
    endif()
  endforeach()
endif()
