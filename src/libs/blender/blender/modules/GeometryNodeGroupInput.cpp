#include "blender/modules/modules.hpp"
#include <absl/status/status.h>
#include <absl/status/statusor.h>
#include <array>
#include <memory>
#include <mir/codegen.hpp>
#include <mir/node_graph/node_graph.hpp>

namespace msk::blender {

auto GenerateTokenStringGeometryNodeGroupInput(Out &out) -> absl::Status {
  auto &left_ports = out.parent.leftPorts;

  for (auto &res : left_ports) {
    out + Out::RIGHT / res.first + " = " + Out::LEFT / res.first + ";" = 1;
  }

  return out.GetStatus();
}

} // namespace msk::blender
