# PLUGIN ABI
add_library(plugin_abi INTERFACE)
target_include_directories(plugin_abi
  INTERFACE
    ${INCLUDES_DIR}/plugin_abi
)

# C library that will fail to build when c++ features are present in the ABI
add_library(abi_header_check STATIC abi_check.c)
target_link_libraries(abi_header_check PRIVATE plugin_abi)

set_target_properties(abi_header_check PROPERTIES
    C_STANDARD 23
    C_STANDARD_REQUIRED ON
)

if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(abi_header_check PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

# IR
add_library(ir_include INTERFACE)
add_library(msk::ir::include ALIAS ir_include)
target_include_directories(ir_include
  INTERFACE
  ${INCLUDES_DIR}/ir
)
target_link_libraries(ir_include
  INTERFACE
  plugin_abi
)

add_library(ir STATIC
  ir/root.cpp
)
add_library(msk::ir ALIAS ir)

add_library(ir_deps INTERFACE)

target_include_directories(ir_deps
  INTERFACE
  ${LIBS_DIR}/ir
)

target_link_libraries(ir_deps
  INTERFACE
    plugin_abi
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

