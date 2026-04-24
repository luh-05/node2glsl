add_executable(viewer main.cpp)

target_link_libraries(viewer PRIVATE
  spdlog
  SDL3
  imgui
  absl::flags
  absl::flags_parse
  absl::flags_usage
)

target_include_directories(viewer PRIVATE
  ABSL_COMMON_INCLUDE_DIRS
)
