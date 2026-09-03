add_executable(viz
  main.cpp
)

add_library(viz_internal_includes INTERFACE)
target_include_directories(viz_internal_includes
  INTERFACE
    ${APPS_DIR}
)
target_link_libraries(viz_internal_includes
  INTERFACE  
    spdlog::spdlog

    absl::status
    absl::statusor
)

target_link_libraries(viz
  PRIVATE
    viz_internal_includes
    
    SDL3::SDL3
    
    absl::flags
    absl::flags_parse
    absl::flags_usage
    
    imgui

    # Modules
    viz::core::util::gpu
    viz::core::util::shader
    viz::ecs
)

add_subdirectory(core)
add_subdirectory(ecs)
