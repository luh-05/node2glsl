add_library(viz_core_state_context)
add_library(viz::core::state::context ALIAS viz_core_state_context)

target_link_libraries(viz_core_state_context
  PRIVATE
    viz_internal_includes
    
    # modules
    viz::core::util::gpu
    viz::core::util::shader
)

target_sources(viz_core_state_context PRIVATE context.cpp)

