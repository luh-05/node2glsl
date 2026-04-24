add_executable(sdl3gpu main.cpp)

target_link_libraries(sdl3gpu PRIVATE
  spdlog
  SDL3::SDL3
  imgui
)

