add_library(blender_modules)
add_library(msk::blender::modules ALIAS blender_modules)

target_include_directories(blender_modules
  PRIVATE
    ${INCLUDES_DIR}/ir
)

target_link_libraries(blender_modules
  PRIVATE
    mollusk_blender_deps
)

target_sources(blender_modules PRIVATE TextToken.cpp WildcardToken.cpp)
