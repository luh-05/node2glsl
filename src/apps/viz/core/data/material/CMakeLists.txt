add_library(viz_core_data_material)
add_library(viz::core::data::material ALIAS viz_core_data_material)

target_link_libraries(viz_core_data_material
  PRIVATE
    viz_internal_includes
    
    # modules
)

target_sources(viz_core_data_material PRIVATE material.cpp)

