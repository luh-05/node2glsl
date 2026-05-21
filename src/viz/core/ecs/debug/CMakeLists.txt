add_library(viz_core_ecs_debug)
add_library(viz::core::ecs::debug ALIAS viz_core_ecs_debug)

target_include_directories(viz_core_ecs_debug
  PRIVATE
    ${CMAKE_SOURCE_DIR}/src/viz
)

target_link_libraries(viz_core_ecs_debug
  PRIVATE
    absl::status
    absl::statusor
    absl::flat_hash_map
    spdlog

    viz::core::geo
    # glm::glm
)

target_sources(viz_core_ecs_debug PRIVATE ecs.cpp)

