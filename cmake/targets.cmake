if(${BUILD_TARGET_NTG_LIB})
  set(BUILD_TARGET_NTG_IR TRUE)
  add_subdirectory(${LIBS_DIR}/core)
endif()

if(${BUILD_TARGET_NTG_IR})
  add_subdirectory(${LIBS_DIR}/ir)
endif()

if(${BUILD_TARGET_BLENDER})
  add_subdirectory(${LIBS_DIR}/blender)
endif()

if(${BUILD_TARGET_NTG_CLI})
  add_subdirectory(${APPS_DIR}/cli)
endif()

if(${BUILD_TARGET_NTG_VIZ})
  add_subdirectory(${APPS_DIR}/viz)
endif()

if(${BUILD_DEMOS}) 
  add_subdirectory(${DEMOS_DIR})
endif()
