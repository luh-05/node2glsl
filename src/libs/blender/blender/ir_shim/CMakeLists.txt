add_library(blender_ir_shim)
add_library(msk::blender::ir_shim ALIAS blender_ir_shim)

target_link_libraries(blender_ir_shim
  PRIVATE
    blender_deps
)

target_sources(blender_ir_shim PRIVATE ir_shim.cpp)
