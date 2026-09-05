add_library(blender
  blender/root.cpp
)
add_library(msk::blender ALIAS blender)

target_include_directories(blender
  PUBLIC
  ${INCLUDES_DIR}/ir
  ${LIBS_DIR}/blender
)

add_library(blender_deps INTERFACE)

target_link_libraries(blender_deps
  INTERFACE
    spdlog::spdlog

    absl::status
    absl::statusor
)

target_link_libraries(blender
  PRIVATE
    blender_deps
  PUBLIC
    msk::blender::modules
)

add_subdirectory(blender/modules)
