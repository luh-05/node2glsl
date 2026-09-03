add_library(viz_core_util_shader)
add_library(viz::core::util::shader ALIAS viz_core_util_shader)


target_link_libraries(viz_core_util_shader
  PRIVATE
    viz_internal_includes

    shaderc
)

target_sources(viz_core_util_shader PRIVATE shader.cpp)
