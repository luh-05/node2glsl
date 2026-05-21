cmake_minimum_required(VERSION 4.1)

project(node2glsl VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(CTest)
include(FetchContent)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${PROJECT_SOURCE_DIR}/bin/debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/bin/release)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG   ${PROJECT_SOURCE_DIR}/bin/debug/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/bin/release/lib)

find_package(Doxygen REQUIRED)

set(DOXYGEN_OUTPUT_DIR ${CMAKE_BINARY_DIR}/docs/doxygen)

configure_file(
  ${CMAKE_SOURCE_DIR}/Doxyfile.in
  ${CMAKE_BINARY_DIR}/Doxyfile
  @ONLY
)

add_custom_target(doc_doxygen
  COMMAND ${CMAKE_COMMAND} -E make_directory ${DOXYGEN_OUTPUT_DIR}
  COMMAND ${CMAKE_COMMAND} ${CMAKE_BINARY_DIR/Doxyfile}
  COMMENT "Generating Doxygen docs"
)

# set(COMPILE_SDL_LOCALLY OFF)

option(COMPILE_SDL_LOCALLY "Should SDL be compiled locally when on UNIX? (Not recommended, requried wayland/x11 libraries to be present on linux)" OFF)

# if (UNIX AND (NOT COMPILE_SDL_LOCALLY)) 
#  find_package(SDL3 REQUIRED)
#
# else ()
#   FetchContent_Declare(
#     SDL3
#     GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
#     GIT_TAG release-3.4.2
#   )
#   FetchContent_MakeAvailable(SDL3)
# endif (UNIX AND (NOT COMPILE_SDL_LOCALLY))

find_package(SDL3 REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(SHADERC REQUIRED IMPORTED_TARGET shaderc)
add_library(shaderc ALIAS PkgConfig::SHADERC)

# abseil
# if (false)
# set(ABSL_PROPAGATE_CXX_STD ON)
# set(ABSL_BUILD_TESTING OFF)
# add_subdirectory(deps/abseil-cpp)
# else ()
find_package(absl CONFIG REQUIRED)
# endif ()

# imgui
add_library(imgui STATIC
  deps/imgui/imgui.cpp
  deps/imgui/imgui_draw.cpp
  deps/imgui/imgui_tables.cpp
  deps/imgui/imgui_widgets.cpp
  deps/imgui/imgui_demo.cpp          # only needed if you call ShowDemoWindow
  # add backend sources when you uncomment SDL3:
  deps/imgui/backends/imgui_impl_sdl3.cpp
  deps/imgui/backends/imgui_impl_sdlrenderer3.cpp
  deps/imgui/backends/imgui_impl_sdlgpu3.cpp
)

target_link_libraries(imgui PRIVATE
  SDL3::SDL3
)

target_include_directories(imgui PUBLIC
  deps/imgui/
  deps/imgui/backends/
)

# spdlog
add_subdirectory(deps/spdlog)

# lw-ecs
add_library(lw-ecs INTERFACE)
target_include_directories(lw-ecs INTERFACE
  deps/lw-ecs/src/include/ECS
)
target_link_libraries(lw-ecs INTERFACE
  absl::flat_hash_set
  absl::container_common
  spdlog
)

# sdl3-shadercross
# find_package(SDL3_shadercross REQUIRED)

# glm
find_package(glm CONFIG REQUIRED)

# demos
add_subdirectory(demos)

# source
add_subdirectory(src)
