add_executable(viz
  main.cpp
)

target_link_libraries(viz
  PRIVATE
    spdlog
    SDL3::SDL3
    imgui
    # SDL3_shadercross
    shaderc 
    absl::flags
    absl::flags_parse
    absl::flags_usage
    # lw-ecs

    # Modules
    viz::core
)

target_include_directories(viz PRIVATE
  ABSL_COMMON_INCLUDE_DIRS
  ${CMAKE_SOURCE_DIR}/src/viz
)

add_subdirectory(core)
