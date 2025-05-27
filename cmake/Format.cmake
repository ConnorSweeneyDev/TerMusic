if(NOT "${IS_SUBDIRECTORY}")
  set(FORMAT_MARKER_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/FormatMarkers")
  file(GLOB FORMAT_FILES "${CMAKE_CURRENT_SOURCE_DIR}/program/**/*.*")
  file(MAKE_DIRECTORY "${FORMAT_MARKER_DIRECTORY}")
  foreach(FILE ${FORMAT_FILES})
    get_filename_component(FILE_NAME "${FILE}" NAME)
    set(FORMAT_FILE "${FORMAT_MARKER_DIRECTORY}/${FILE_NAME}.format")
    add_custom_command(
      OUTPUT "${FORMAT_FILE}"
      COMMAND clang-format -i "${FILE}"
      COMMAND "${CMAKE_COMMAND}" -E touch "${FORMAT_FILE}"
      DEPENDS "${FILE}"
    )
    list(APPEND FORMAT_MARKERS "${FORMAT_FILE}")
  endforeach()
  add_custom_target("Format" DEPENDS "${FORMAT_MARKERS}")
  list(APPEND DEPENDENCIES "Format")
endif()
