cmake_minimum_required(VERSION 4.1)

project(node2glsl VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(CTest)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${PROJECT_SOURCE_DIR}/bin/debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/bin/release)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG   ${PROJECT_SOURCE_DIR}/bin/debug/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/bin/release/lib)

# Static lib are intermediate artifacts and not required in binary
# CMAKE_ARCHIVE_OUTPUT_DIRECTORY intentionally unset

find_package(SDL3 REQUIRED)
# find_package(dearimgui REQUIRED)

# abseil
set(ABSL_PROPAGATE_CXX_STD ON)
set(ABSL_BUILD_TESTING OFF)
add_subdirectory(deps/abseil-cpp)

# source
add_subdirectory(src)
