add_library(viz_core_util_math INTERFACE)
add_library(viz::core::util::math ALIAS viz_core_util_math)

target_link_libraries(viz_core_util_math
  INTERFACE
    viz_internal_includes

    glm::glm
)

target_precompile_headers(viz_core_util_math INTERFACE pch.hpp)

# target_sources(viz_core_util_math PRIVATE mesh.cpp)

