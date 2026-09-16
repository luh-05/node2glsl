add_library(blender_modules)
add_library(msk::blender::modules ALIAS blender_modules)

target_link_libraries(blender_modules PRIVATE blender_deps)

target_sources(
  blender_modules
  PRIVATE
    ./DummyModule.cpp
    ./FunctionNodeBitMathModule.cpp
    ./FunctionNodeCompareModule.cpp
    ./FunctionNodeHashValueModule.cpp
    ./FunctionNodeIntegerMathModule.cpp
    ./FunctionNodeFloatToIntMathModule.cpp
    ./ShaderNodeBooleanMathModule.cpp
    ./ShaderNodeClampModule.cpp
    ./ShaderNodeFloatCurveModule.cpp
    ./ShaderNodeMapRangeModule.cpp
    ./ShaderNodeMathModule.cpp
    ./ShaderNodeMixModule.cpp
)
