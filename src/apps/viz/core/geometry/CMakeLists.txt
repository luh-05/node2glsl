add_library(viz_core_geo)
add_library(viz::core::geo ALIAS viz_core_geo)

target_include_directories(viz_core_geo
  PRIVATE
  ${APPS_DIR}/viz
)

target_link_libraries(viz_core_geo
  PRIVATE
    absl::status
    absl::statusor
    glm::glm
)

target_precompile_headers(viz_core_geo PRIVATE pch.hpp)

target_sources(viz_core_geo PRIVATE mesh.cpp)

