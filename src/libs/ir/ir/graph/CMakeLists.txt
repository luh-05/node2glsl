add_library(mir_graph)
add_library(mir::graph ALIAS mir_graph)

target_link_libraries(mir_graph
  PRIVATE
    ir_deps
)

target_sources(mir_graph PUBLIC GraphContext.cpp Module.cpp)
