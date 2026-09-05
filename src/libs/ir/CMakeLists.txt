add_library(ir
  ir/root.cpp
)
add_library(msk::ir ALIAS ir)

target_include_directories(ir
  PUBLIC
  ${INCLUDES_DIR}/ir
  ${LIBS_DIR}/ir
)

add_library(ir_deps INTERFACE)

target_link_libraries(ir_deps
  INTERFACE
    spdlog::spdlog

    absl::status
    absl::statusor
    absl::strings
)

target_link_libraries(ir
  PRIVATE
    ir_deps
  PUBLIC
    mir::codegen
    mir::graph
)

add_subdirectory(ir/codegen)
add_subdirectory(ir/graph)
