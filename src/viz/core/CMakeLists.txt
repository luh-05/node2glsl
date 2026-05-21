add_library(viz_core)
add_library(viz::core ALIAS viz_core)

target_include_directories(viz_core
  PRIVATE
    ${CMAKE_SOURCE_DIR}/src/viz
)

target_link_libraries(viz_core
  PRIVATE
    spdlog
    absl::status
    absl::statusor
    glm

    # Modules
    viz::core::shader
    viz::core::geo
    viz::core::ecs
)

target_sources(viz_core PRIVATE gpu.cpp)

add_subdirectory(shader)
add_subdirectory(geometry)
add_subdirectory(ecs)
