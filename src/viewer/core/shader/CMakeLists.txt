add_library(viz_core_shader)
add_library(viz::core::shader ALIAS viz_core_shader)

target_include_directories(viz_core_shader
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(viz_core_shader
  PRIVATE
    spdlog
    PkgConfig::SHADERC
)

target_sources(viz_core_shader PRIVATE shader.cpp)
