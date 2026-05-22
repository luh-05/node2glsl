add_library(viz_core_shader)
add_library(viz::core::shader ALIAS viz_core_shader)


target_link_libraries(viz_core_shader
  PRIVATE
    viz_internal_includes
    spdlog::spdlog

    shaderc
)

target_sources(viz_core_shader PRIVATE shader.cpp)
