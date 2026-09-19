#include "blender/root.hpp"
#include "blender/modules/modules.hpp"
#include "mir/node_graph/node_graph.hpp"
#include <plugin_abi.h>

namespace msk::blender {
template <msk::ir::Module::GenerateTokenString func>
size_t FuncWrapper(void *out) {
  auto s = func(*static_cast<ir::Module::Out *>(out));
  return s.raw_code();
}
} // namespace msk::blender

void EnumerateModules(PluginModuleCallback callback, void *userdata) {
  using namespace msk::blender;

  callback("DummyModule", FuncWrapper<GenerateTokenStringDummy>, userdata);
  callback("FunctionNodeBitMath",
           FuncWrapper<GenerateTokenStringFunctionNodeBitMath>, userdata);
  callback("FunctionNodeBooleanMath",
           FuncWrapper<GenerateTokenStringFunctionNodeBooleanMath>, userdata);
  callback("FunctionNodeCompare",
           FuncWrapper<GenerateTokenStringFunctionNodeCompare>, userdata);
  callback("FunctionNodeFloatToInt",
           FuncWrapper<GenerateTokenStringFunctionNodeFloatToIntMath>,
           userdata);
  callback("FunctionNodeHashValue",
           FuncWrapper<GenerateTokenStringFunctionNodeHashValue>, userdata);
  callback("FunctionNodeIntegerMath",
           FuncWrapper<GenerateTokenStringFunctionNodeIntegerMath>, userdata);
  callback("ShaderNodeClamp", FuncWrapper<GenerateTokenStringShaderNodeClamp>,
           userdata);
  callback("ShaderNodeFloatCurve",
           FuncWrapper<GenerateTokenStringShaderNodeFloatCurve>, userdata);
  callback("ShaderNodeMapRange",
           FuncWrapper<GenerateTokenStringShaderNodeMapRange>, userdata);
  callback("ShaderNodeMath", FuncWrapper<GenerateTokenStringShaderNodeMath>,
           userdata);
  callback("ShaderNodeMix", FuncWrapper<GenerateTokenStringShaderNodeMix>,
           userdata);
  callback("FunctionNodeInputBool",
           FuncWrapper<GenerateTokenStringFunctionNodeInputBool>, userdata);
  callback("FunctionNodeInputInt",
           FuncWrapper<GenerateTokenStringFunctionNodeInputInt>, userdata);
  callback("FunctionNodeInputRotation",
           FuncWrapper<GenerateTokenStringFunctionNodeInputRotation>, userdata);
  callback("FunctionNodeInputVector",
           FuncWrapper<GenerateTokenStringFunctionNodeInputVector>, userdata);
  callback("ShaderNodeValue", FuncWrapper<GenerateTokenStringShaderNodeValue>,
           userdata);
}
