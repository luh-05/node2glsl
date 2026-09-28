add_executable(mollusk_cli
  ./main.cpp
)
target_include_directories(mollusk_cli
  PUBLIC
    .
)
target_link_libraries(mollusk_cli
  PRIVATE
    spdlog::spdlog

    absl::status
    absl::statusor
    absl::flags
    absl::flags_usage
    absl::flags_parse

    msk::ir
    msk::blender
    msk::core
)
