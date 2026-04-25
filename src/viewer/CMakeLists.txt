add_executable(viewer
  main.cpp
  shader.cpp
)

target_link_libraries(viewer PRIVATE
  spdlog
  SDL3::SDL3
  imgui
  # SDL3_shadercross
  PkgConfig::SHADERC
  absl::flags
  absl::flags_parse
  absl::flags_usage
  lw-ecs
)

target_include_directories(viewer PRIVATE
  ABSL_COMMON_INCLUDE_DIRS
)
