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

bool EnumerateModules(PluginModuleCallback callback, void *userdata) {
  using namespace msk::blender;

  if (!callback("DummyModule", FuncWrapper<GenerateTokenStringDummy>, userdata))
    return false;
  if (!callback("FunctionNodeBitMath",
                FuncWrapper<GenerateTokenStringFunctionNodeBitMath>, userdata))
    return false;
  if (!callback("FunctionNodeBooleanMath",
                FuncWrapper<GenerateTokenStringFunctionNodeBooleanMath>,
                userdata))
    return false;
  if (!callback("FunctionNodeCompare",
                FuncWrapper<GenerateTokenStringFunctionNodeCompare>, userdata))
    return false;
  if (!callback("FunctionNodeFloatToInt",
                FuncWrapper<GenerateTokenStringFunctionNodeFloatToInt>,
                userdata))
    return false;
  if (!callback("FunctionNodeHashValue",
                FuncWrapper<GenerateTokenStringFunctionNodeHashValue>,
                userdata))
    return false;
  if (!callback("FunctionNodeIntegerMath",
                FuncWrapper<GenerateTokenStringFunctionNodeIntegerMath>,
                userdata))
    return false;
  if (!callback("NodeGroupOutput",
                FuncWrapper<GenerateTokenStringNodeGroupOutput>, userdata))
    return false;
  if (!callback("ShaderNodeClamp",
                FuncWrapper<GenerateTokenStringShaderNodeClamp>, userdata))
    return false;
  if (!callback("ShaderNodeFloatCurve",
                FuncWrapper<GenerateTokenStringShaderNodeFloatCurve>, userdata))
    return false;
  if (!callback("ShaderNodeMapRange",
                FuncWrapper<GenerateTokenStringShaderNodeMapRange>, userdata))
    return false;
  if (!callback("ShaderNodeMath",
                FuncWrapper<GenerateTokenStringShaderNodeMath>, userdata))
    return false;
  if (!callback("ShaderNodeMix", FuncWrapper<GenerateTokenStringShaderNodeMix>,
                userdata))
    return false;
  if (!callback("FunctionNodeInputBool",
                FuncWrapper<GenerateTokenStringFunctionNodeInputBool>,
                userdata))
    return false;
  if (!callback("FunctionNodeInputInt",
                FuncWrapper<GenerateTokenStringFunctionNodeInputInt>, userdata))
    return false;
  if (!callback("FunctionNodeInputRotation",
                FuncWrapper<GenerateTokenStringFunctionNodeInputRotation>,
                userdata))
    return false;
  if (!callback("FunctionNodeInputVector",
                FuncWrapper<GenerateTokenStringFunctionNodeInputVector>,
                userdata))
    return false;
  if (!callback("ShaderNodeValue",
                FuncWrapper<GenerateTokenStringShaderNodeValue>, userdata))
    return false;
  return true;
}

void GetInfo(PluginInfo *info) {
  std::strncpy(info->id, "com.official.blender\0", PLUGIN_ID_MAX);
  std::strncpy(info->name, "Official Blender Plugin for XML import\0",
               PLUGIN_NAME_MAX);
}
