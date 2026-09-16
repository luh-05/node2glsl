#include "mir/node_graph/node_graph.hpp"
#include "modules.hpp"
#include <absl/status/status.h>
#include <absl/status/statusor.h>
#include <array>
#include <memory>
#include <string>
#include <mir/codegen.hpp>

namespace msk::blender {

auto FunctionNodeHashValueModule::GenerateTokenString(Out &&out) -> absl::Status {
  auto data_type = out.GetConstant<std::string>("data_type0");

  std::string glsl_func;

  if (data_type == "FLOAT") {
    glsl_func = "hash_float";
  } else if (data_type == "INT") {
    glsl_func = "hash_int";
  } else if (data_type == "VECTOR") {
    glsl_func = "hash_vec3";
  } else {
    return absl::InvalidArgumentError(
      std::format("Unsupported data_type for Hash Value: '{}'", data_type));
  }

  //FIXME: Implement GLSL Helper Function

  out + Out::RIGHT / "Hash0"
    + " = " + glsl_func + "(" 
    + Out::LEFT / "Value0" 
    + ", " 
    + Out::LEFT / "Seed0" 
    + ");";

  return out.GetStatus();
} 

} // namespace msk::blender
