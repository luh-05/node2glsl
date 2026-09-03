add_library(mollusk_blender
  root.cpp
)
add_library(msk::blender ALIAS mollusk_blender)

target_include_directories(mollusk_blender
  PUBLIC
  ${INCLUDES_DIR}/ir
  PRIVATE
  ${LIBS_DIR}
)

add_library(mollusk_blender_deps
  INTERFACE
    spdlog::spdlog

    absl::status
    absl::statusor
)

target_link_libraries(mollusk_blender
  PRIVATE
    mollusk_blender_deps
    msk::blender::modules
)
