add_library(blender
  blender/root.cpp
)
add_library(msk::blender ALIAS blender)

add_library(blender_deps INTERFACE)

target_include_directories(blender
  PUBLIC
    ${LIBS_DIR}/blender 
)

target_include_directories(blender_deps
  INTERFACE
    ${LIBS_DIR}/blender
)

target_link_libraries(blender_deps
  INTERFACE
    msk::ir::include
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
