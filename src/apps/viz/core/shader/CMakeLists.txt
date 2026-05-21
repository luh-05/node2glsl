add_library(viz_core_shader)
add_library(viz::core::shader ALIAS viz_core_shader)

target_include_directories(viz_core_shader
  PUBLIC
  ${APPS_DIR}/viz
)

target_link_libraries(viz_core_shader
  PRIVATE
    spdlog
    shaderc
    absl::status
    absl::statusor
)

target_sources(viz_core_shader PRIVATE shader.cpp)
