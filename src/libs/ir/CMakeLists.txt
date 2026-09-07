add_library(ir_include INTERFACE)
add_library(msk::ir::include ALIAS ir_include)
target_include_directories(ir_include
  INTERFACE
  ${INCLUDES_DIR}/ir
)

add_library(ir
  ir/root.cpp
)
add_library(msk::ir ALIAS ir)

add_library(ir_deps INTERFACE)

target_include_directories(ir
  INTERFACE
  ${LIBS_DIR}/ir
)
target_include_directories(ir_deps
  INTERFACE
  ${LIBS_DIR}/ir
)

target_link_libraries(ir_deps
  INTERFACE
    msk::ir::include
    spdlog::spdlog

    absl::status
    absl::statusor
)

target_link_libraries(ir
  PRIVATE
    ir_deps
  PUBLIC
    msk::ir::include
    mir::codegen
    mir::graph
)

add_subdirectory(ir/codegen)
add_subdirectory(ir/graph)
