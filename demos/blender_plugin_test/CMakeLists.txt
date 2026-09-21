add_executable(blenderplugintest main.cpp)

target_link_libraries(blenderplugintest PRIVATE
  msk::core
  msk::ir
  msk::blender
)

