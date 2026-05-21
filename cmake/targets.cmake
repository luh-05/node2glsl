if(${BUILD_TARGET_NTG_LIB})
  add_subdirectory(${LIBS_DIR}/ntg)
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
