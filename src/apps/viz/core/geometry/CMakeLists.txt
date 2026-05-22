add_library(viz_core_geo)
add_library(viz::core::geo ALIAS viz_core_geo)

target_link_libraries(viz_core_geo
  PRIVATE
    viz_internal_includes

    glm::glm
)

target_precompile_headers(viz_core_geo PRIVATE pch.hpp)

target_sources(viz_core_geo PRIVATE mesh.cpp)

