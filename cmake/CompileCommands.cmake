if(NOT "${IS_SUBDIRECTORY}")
  set(COMPILE_COMMANDS_FILE "${CMAKE_CURRENT_SOURCE_DIR}/compile_commands.json")
  file(WRITE "${COMPILE_COMMANDS_FILE}" "[\n")
  foreach(FILE ${SOURCE_FILES})
    list(LENGTH SOURCE_FILES NUM_SOURCE_FILES)
    get_filename_component(FILE_EXT "${FILE}" EXT)
    string(FIND "${FILE}" "/program/source/" POS)
    if(NOT "${POS}" EQUAL "-1")
      set(IS_EXTERNAL OFF)
    else()
      set(IS_EXTERNAL ON)
    endif()
    if("${FILE_EXT}" MATCHES "cpp")
      file(
        APPEND
        "${COMPILE_COMMANDS_FILE}"
        "  {\n    \"directory\": \"${CMAKE_CURRENT_SOURCE_DIR}\",\n    \"file\": \"${FILE}\",\n    \"command\": \"clang++ -std=c++${CMAKE_CXX_STANDARD} "
      )
    else()
      file(
        APPEND
        "${COMPILE_COMMANDS_FILE}"
        "  {\n    \"directory\": \"${CMAKE_CURRENT_SOURCE_DIR}\",\n    \"file\": \"${FILE}\",\n    \"command\": \"clang "
      )
    endif()
    if(NOT ${IS_EXTERNAL})
      file(
        APPEND
        "${COMPILE_COMMANDS_FILE}"
        "-Wall -Wextra -Wpedantic -Wconversion -Wshadow-all -Wundef -Wdeprecated -Wtype-limits -Wcast-qual -Wcast-align -Wfloat-equal -Wparentheses -Wunreachable-code-aggressive -Wformat=2 "
      )
    endif()
    foreach(DIRECTORY ${INCLUDE_DIRECTORIES})
      file(APPEND "${COMPILE_COMMANDS_FILE}" "-I\\\"${DIRECTORY}\\\" ")
    endforeach()
    foreach(DIRECTORY ${SYSTEM_INCLUDE_DIRECTORIES})
      file(APPEND "${COMPILE_COMMANDS_FILE}" "-isystem\\\"${DIRECTORY}\\\" ")
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
