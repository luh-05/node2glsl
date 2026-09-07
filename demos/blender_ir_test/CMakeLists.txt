add_executable(blenderirtest main.cpp)

target_link_libraries(blenderirtest PRIVATE
  msk::ir
  msk::blender
)

