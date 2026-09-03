add_library(viz_core INTERFACE)
add_library(viz::core ALIAS viz_core)

target_link_libraries(viz_core
  INTERFACE
    viz_internal_includes

    # Modules
    # viz::core::data
    # viz::core::render
    # viz::core::state
    # viz::core::util
)

# target_sources(viz_core PRIVATE gpu.cpp)

add_subdirectory(util)
add_subdirectory(state)
