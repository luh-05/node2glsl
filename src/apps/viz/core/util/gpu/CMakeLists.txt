add_library(viz_core_util_gpu)
add_library(viz::core::util::gpu ALIAS viz_core_util_gpu)


target_link_libraries(viz_core_util_gpu 
  PRIVATE
    viz_internal_includes

    shaderc
)

target_sources(viz_core_util_gpu PRIVATE gpu.cpp)
