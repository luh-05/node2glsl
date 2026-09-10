add_executable(blendergraphtest main.cpp)

target_link_libraries(blendergraphtest PRIVATE
  msk::ir
  msk::blender
)

