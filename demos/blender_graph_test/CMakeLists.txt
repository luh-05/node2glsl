add_executable(blendergraphtest main.cpp)

target_link_libraries(blendergraphtest PRIVATE
  msk::core
  msk::ir
  msk::blender
)

