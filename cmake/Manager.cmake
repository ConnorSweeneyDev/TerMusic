set(CPM_VERSION "0.42.0")
set(VCPKG_VERSION "2025.04.09")

set(CPM_DONT_CREATE_PACKAGE_LOCK ON CACHE BOOL "")
file(
  DOWNLOAD
    "https://github.com/cpm-cmake/CPM.cmake/releases/download/v${CPM_VERSION}/CPM.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake"
)
include("${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake")

set(VCPKG_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/vcpkg")
set(VCPKG_INSTALLED_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/_deps/vcpkg_installed")
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
    COMMAND cmd /C git checkout ${VCPKG_VERSION} > NUL 2>&1
    WORKING_DIRECTORY "${VCPKG_DIRECTORY}"
  )
  execute_process(COMMAND cmd /C bootstrap-vcpkg.bat -disableMetrics WORKING_DIRECTORY "${VCPKG_DIRECTORY}")
endif()
