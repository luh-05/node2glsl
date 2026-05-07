add_library(viz_core INTERFACE)
add_library(viz::core ALIAS viz_core)

target_include_directories(viz_core
  INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(viz_core
  INTERFACE
    viz::core::shader
)

add_subdirectory(shader)
