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
      {"GeometryNodeGroupInput",
       FuncWrapper<GenerateTokenStringGeometryNodeGroupInput>},
      {"GeometryNodeGroupOutput",
       FuncWrapper<GenerateTokenStringGeometryNodeGroupOutput>},
      {"NodeGroupInput", FuncWrapper<GenerateTokenStringNodeGroupInput>},
      {"NodeGroupOutput", FuncWrapper<GenerateTokenStringNodeGroupOutput>},
      {"ShaderNodeClamp", FuncWrapper<GenerateTokenStringShaderNodeClamp>},
      {"ShaderNodeFloatCurve",
       FuncWrapper<GenerateTokenStringShaderNodeFloatCurve>},
      {"ShaderNodeMapRange",
       FuncWrapper<GenerateTokenStringShaderNodeMapRange>},
      {"ShaderNodeMath", FuncWrapper<GenerateTokenStringShaderNodeMath>},
      {"ShaderNodeSeperaeXYZ",
       FuncWrapper<GenerateTokenStringShaderNodeSeperateXYZ>},
      {"ShaderNodeVectorMath",
       FuncWrapper<GenerateTokenStringShaderNodeVectorMath>},
      {"ShaderNodeMix", FuncWrapper<GenerateTokenStringShaderNodeMix>},
      {"FunctionNodeInputBool",
       FuncWrapper<GenerateTokenStringFunctionNodeInputBool>},
      {"FunctionNodeInputInt",
       FuncWrapper<GenerateTokenStringFunctionNodeInputInt>},
      {"FunctionNodeInputRotation",
       FuncWrapper<GenerateTokenStringFunctionNodeInputRotation>},
      {"FunctionNodeInputVector",
       FuncWrapper<GenerateTokenStringFunctionNodeInputVector>},
      {"FunctionNodeInputString",
       FuncWrapper<GenerateTokenStringFunctionNodeInputString>},
      {"ShaderNodeValue", FuncWrapper<GenerateTokenStringShaderNodeValue>}};

  for (const auto &[name, func] : impls) {
    if (!callback(name, func, userdata))
      return false;
  }
  return true;
}

void EnumerateDefinitions(PluginDefinitionCallback callback, void *userdata) {
  callback("VALUE float", userdata);
  callback("VECTOR vec3", userdata);
  callback("RGBA vec4", userdata);
  callback("ROTATION vec3", userdata);
  callback("BOOLEAN bool", userdata);
  callback("STRING void", userdata);
}

void EnumerateCasts(CastPolicyCallback callback, void *userdata) {
  constexpr std::tuple<const char *, const char *, const char *> policies[] = {
      {"VECTOR", "VALUE", "length({})"},
      {"VALUE", "VECTOR", "vec3({0:}, {0:}, {0:})"},
      {"VALUE", "RGBA", "vec4({0:}, {0:}, {0:}, 1.0f)"},
      {"VALUE", "ROTATION", "vec3({0:}, {0:}, {0:})"},
  };

  for (const auto &[from, to, pattern] : policies) {
    callback(from, to, pattern, userdata);
  }
}

void GetInfo(PluginInfo *info) {
  std::strncpy(info->id, "com.official.blender\0", PLUGIN_ID_MAX);
  std::strncpy(info->name, "Official Blender Plugin for XML import\0",
               PLUGIN_NAME_MAX);
}
