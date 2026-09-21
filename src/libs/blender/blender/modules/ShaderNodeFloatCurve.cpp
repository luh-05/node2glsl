#include "mir/node_graph/node_graph.hpp"
#include "modules.hpp"
#include <absl/status/status.h>
#include <absl/status/statusor.h>
#include <array>
#include <memory>
#include <mir/codegen.hpp>
#include <string>

namespace msk::blender {

auto GenerateTokenStringShaderNodeFloatCurve(Out &out) -> absl::Status {

  // FIXME: implement GLSL Helperfunction
  return absl::UnimplementedError("Function has not been implemented yet.");
}

} // namespace msk::blender
