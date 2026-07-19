add_executable(irtest main.cpp)

target_link_libraries(irtest PRIVATE
  msk::ir 
  ir_deps
)

