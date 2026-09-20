#include "blender/root.hpp"
#include "blender/modules/modules.hpp"
#include "mir/node_graph/node_graph.hpp"
#include "plugin_abi/plugin_abi.h"
#include <cstring>

namespace msk::blender {
template <msk::ir::Module::GenerateTokenString func>
void FuncWrapper(void *out, PluginStatus *status) {
  auto s = func(*static_cast<ir::Module::Out *>(out));
  status->errc = s.raw_code();
  auto msg = s.ToString();
  strncpy(status->message, msg.data(), PLUGIN_ERROR_MAX);
  status->message[msg.length() < PLUGIN_ERROR_MAX - 1 ? msg.length()
                                                      : PLUGIN_ERROR_MAX - 1] =
      '\0';
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
           FuncWrapper<GenerateTokenStringFunctionNodeFloatToInt>, userdata);
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

void GetInfo(PluginInfo *info) {
  std::strncpy(info->id, "com.official.blender\0", PLUGIN_ID_MAX);
  std::strncpy(info->name, "Official Blender Plugin for XML import\0",
               PLUGIN_NAME_MAX);
}
