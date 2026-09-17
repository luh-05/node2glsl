add_library(core_include INTERFACE)
add_library(msk::include ALIAS core_include)
target_include_directories(core_include
  INTERFACE
    ${INCLUDES_DIR}/core
)

add_library(mollusk_core
  mollusk/root.cpp
)
add_library(msk::core ALIAS mollusk_core)

add_library(core_deps INTERFACE)

target_include_directories(core_deps
  INTERFACE
    ${LIBS_DIR}/core
)

target_link_libraries(core_deps
  INTERFACE
    msk::include
    spdlog::spdlog

    absl::status
    absl::statusor

    msk::ir
    msk::blender
)

target_link_libraries(mollusk_core
  PRIVATE
    core_deps
  PUBLIC
    msk::include
    msk::eval_strats
)

add_subdirectory(mollusk/evaluation_strategies)
