find_package(PkgConfig REQUIRED)

# add_subdirectory(${DEPS_DIR}/spdlog)
find_package(spdlog REQUIRED)

find_package(SDL3 REQUIRED)

pkg_check_modules(SHADERC REQUIRED IMPORTED_TARGET shaderc)
add_library(shaderc ALIAS PkgConfig::SHADERC)

find_package(absl CONFIG REQUIRED)

# find_package(imgui REQUIRED)
add_library(imgui STATIC
  ${DEPS_DIR}/imgui/imgui.cpp
  ${DEPS_DIR}/imgui/imgui_draw.cpp
  ${DEPS_DIR}/imgui/imgui_tables.cpp
  ${DEPS_DIR}/imgui/imgui_widgets.cpp
  ${DEPS_DIR}/imgui/imgui_demo.cpp
  ${DEPS_DIR}/imgui/backends/imgui_impl_sdl3.cpp
  ${DEPS_DIR}/imgui/backends/imgui_impl_sdlrenderer3.cpp
  ${DEPS_DIR}/imgui/backends/imgui_impl_sdlgpu3.cpp
)

target_link_libraries(imgui PRIVATE
  SDL3::SDL3
)

target_include_directories(imgui PUBLIC
  ${DEPS_DIR}/imgui/
  ${DEPS_DIR}/imgui/backends/
)

find_package(glm CONFIG REQUIRED)
