add_library(viz_core_ecs)
add_library(viz::core::ecs ALIAS viz_core_ecs)

target_link_libraries(viz_core_ecs
  PRIVATE
    viz_internal_includes

    absl::flat_hash_map
)

target_sources(viz_core_ecs PRIVATE ecs.cpp)

