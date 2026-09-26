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

  constexpr std::pair<const char *, ModuleFunc> impls[] = {
      {"DummyModule", FuncWrapper<GenerateTokenStringDummy>},
      {"FunctionNodeBitMath",
       FuncWrapper<GenerateTokenStringFunctionNodeBitMath>},
      {"FunctionNodeBooleanMath",
       FuncWrapper<GenerateTokenStringFunctionNodeBooleanMath>},
      {"FunctionNodeCompare",
       FuncWrapper<GenerateTokenStringFunctionNodeCompare>},
      {"FunctionNodeFloatToInt",
       FuncWrapper<GenerateTokenStringFunctionNodeFloatToInt>},
      {"FunctionNodeHashValue",
       FuncWrapper<GenerateTokenStringFunctionNodeHashValue>},
      {"FunctionNodeIntegerMath",
       FuncWrapper<GenerateTokenStringFunctionNodeIntegerMath>},
      {"NodeGroupOutput", FuncWrapper<GenerateTokenStringNodeGroupOutput>},
      {"ShaderNodeClamp", FuncWrapper<GenerateTokenStringShaderNodeClamp>},
      {"ShaderNodeFloatCurve",
       FuncWrapper<GenerateTokenStringShaderNodeFloatCurve>},
      {"ShaderNodeMapRange",
       FuncWrapper<GenerateTokenStringShaderNodeMapRange>},
      {"ShaderNodeMath", FuncWrapper<GenerateTokenStringShaderNodeMath>},
      {"ShaderNodeMix", FuncWrapper<GenerateTokenStringShaderNodeMix>},
      {"FunctionNodeInputBool",
       FuncWrapper<GenerateTokenStringFunctionNodeInputBool>},
      {"FunctionNodeInputInt",
       FuncWrapper<GenerateTokenStringFunctionNodeInputInt>},
      {"FunctionNodeInputRotation",
       FuncWrapper<GenerateTokenStringFunctionNodeInputRotation>},
      {"FunctionNodeInputVector",
       FuncWrapper<GenerateTokenStringFunctionNodeInputVector>},
      {"ShaderNodeValue", FuncWrapper<GenerateTokenStringShaderNodeValue>}};

  for (const auto &[name, func] : impls) {
    if (!callback(name, func, userdata))
      return false;
  }
  return true;
}

void EnumerateDefinitions(PluginDefinitionCallback callback, void *userdata) {
  callback("VALUE float", userdata);
}

void GetInfo(PluginInfo *info) {
  std::strncpy(info->id, "com.official.blender\0", PLUGIN_ID_MAX);
  std::strncpy(info->name, "Official Blender Plugin for XML import\0",
               PLUGIN_NAME_MAX);
}
