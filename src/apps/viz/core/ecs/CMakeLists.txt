add_library(viz_core_ecs)
add_library(viz::core::ecs ALIAS viz_core_ecs)

target_include_directories(viz_core_ecs
  PRIVATE
  ${APPS_DIR}/viz
)

target_link_libraries(viz_core_ecs
  PRIVATE
    absl::status
    absl::statusor
    absl::flat_hash_map
    spdlog

    # viz::core::geo
    # glm::glm
)

target_sources(viz_core_ecs PRIVATE ecs.cpp)

