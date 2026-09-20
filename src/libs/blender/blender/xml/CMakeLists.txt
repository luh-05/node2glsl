add_library(blender_xml)
add_library(msk::blender::xml ALIAS blender_xml)

target_link_libraries(blender_xml
  PRIVATE
    blender_deps
    pugixml
    blender
    msk::blender::ir_shim
    msk::blender::modules
)

target_sources(blender_xml PRIVATE xml.cpp)
