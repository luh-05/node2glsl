add_library(mir_graph)
add_library(mir::graph ALIAS mir_graph)

target_include_directories(mir_graph
  PUBLIC
    ${INCLUDES_DIR}/ir
    ${LIBS_DIR}/ir
)

target_link_libraries(mir_graph
  PRIVATE
    ir_deps
)

target_sources(mir_graph PUBLIC GraphContext.cpp)
