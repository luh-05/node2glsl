add_executable(blenderforwardevaltest main.cpp)

target_link_libraries(blenderforwardevaltest PRIVATE
  msk::core
  msk::ir
  msk::blender
)

