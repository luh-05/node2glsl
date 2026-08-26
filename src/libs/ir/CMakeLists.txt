add_library(ir
  root.cpp
)
add_library(msk::ir ALIAS ir)

target_include_directories(ir
  PUBLIC
  ${INCLUDES_DIR}/ir 
)

add_library(ir_deps INTERFACE)

target_link_libraries(ir_deps
  INTERFACE
    spdlog::spdlog

    absl::status
    absl::statusor
)

target_link_libraries(ir
  PRIVATE
    ir_deps
    mir::codegen
)

add_subdirectory(codegen)
