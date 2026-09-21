
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${PROJECT_SOURCE_DIR}/bin/debug/demo)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/bin/release/demo)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG   ${PROJECT_SOURCE_DIR}/bin/debug/demo/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/bin/release/demo/lib)

if(${BUILD_TARGET_NTG_VIZ})
  add_subdirectory(sdl3)
  add_subdirectory(sdl3gpu)
endif()
add_subdirectory(ir_test)
add_subdirectory(blender_ir_test)
add_subdirectory(blender_graph_test)
add_subdirectory(blender_forwardeval_test)
add_subdirectory(blender_plugin_test)
