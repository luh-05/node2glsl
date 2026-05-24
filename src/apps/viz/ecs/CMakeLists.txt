add_library(viz_ecs)
add_library(viz::ecs ALIAS viz_ecs)

target_link_libraries(viz_ecs
  PRIVATE
    viz_internal_includes

    absl::flat_hash_map

    # modules
    viz::core::state::context
)

target_sources(viz_ecs PRIVATE ecs.cpp)

