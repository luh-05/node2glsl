find_package(SDL3 REQUIRED)

pkg_check_modules(SHADERC REQUIRED IMPORTED_TARGET shaderc)
add_library(shaderc ALIAS PkgConfig::SHADERC)

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



add_executable(viz
  main.cpp
)

add_library(viz_internal_includes INTERFACE)
target_include_directories(viz_internal_includes
  INTERFACE
    ${APPS_DIR}
)
target_link_libraries(viz_internal_includes
  INTERFACE  
    spdlog::spdlog

    absl::status
    absl::statusor
)

target_link_libraries(viz
  PRIVATE
    viz_internal_includes
    
    SDL3::SDL3
    
    absl::flags
    absl::flags_parse
    absl::flags_usage
    
    imgui

    # Modules
    viz::core::util::gpu
    viz::core::util::shader
    viz::ecs
)

add_subdirectory(core)
add_subdirectory(ecs)
