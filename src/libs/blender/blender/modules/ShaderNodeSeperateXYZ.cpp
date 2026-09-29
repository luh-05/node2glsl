#include "blender/modules/modules.hpp"
#include <absl/status/status.h>
#include <absl/status/statusor.h>
#include <array>
#include <memory>
#include <mir/codegen.hpp>
#include <mir/node_graph/node_graph.hpp>

namespace msk::blender {

auto GenerateTokenStringShaderNodeSeperateXYZ(Out &out) -> absl::Status {

  out + Out::RIGHT / "X0" + "=" + Out::LEFT / "Vector0" + ".x;" = 1;
  out + Out::RIGHT / "Y0" + "=" + Out::LEFT / "Vector0" + ".y;" = 1;
  out + Out::RIGHT / "Z0" + "=" + Out::LEFT / "Vector0" + ".z;" = 1;

  return out.GetStatus();
}

} // namespace msk::blender
