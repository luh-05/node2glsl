add_library(viz_core_util_math_geometry)
add_library(viz::core::util::math::geometry ALIAS viz_core_util_math_geometry)

target_link_libraries(viz_core_util_math_math_geometry
  PRIVATE
    viz_internal_includes

    glm::glm
)

target_sources(viz_core_util_math_geometry PRIVATE mesh.cpp)

