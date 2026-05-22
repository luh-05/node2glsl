add_executable(sdl3window main.cpp)

target_link_libraries(sdl3window PRIVATE
  spdlog::spdlog
  SDL3::SDL3
  imgui
)

