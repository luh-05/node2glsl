#include "blender/modules/modules.hpp"
#include <absl/status/status.h>
#include <absl/status/statusor.h>
#include <array>
#include <memory>
#include <mir/codegen.hpp>
#include <mir/node_graph/node_graph.hpp>

namespace msk::blender {

auto GenerateTokenStringNodeGroupInput(Out &out) -> absl::Status {
  constexpr std::tuple<const char *, const char *, const char *> magic_names[] =
      {
          {"p3d_position0", "p3d_position", "VECTOR"},
          {"p3d_voxelSize0", "p3d_voxelSize", "VECTOR"},
          {"p3d_voxelGridSize0", "p3d_voxelGridSize", "VECTOR"},
          {"p3d_sliceOffset0", "p3d_sliceOffset", "VECTOR"},
          {"p3d_sliceIndex0", "p3d_sliceIndex", "INT"},
          {"p3d_vol_color0", "p3d_vol_color", "COLOR"},
      };
  auto &right_ports = out.parent.rightPorts;
  for (auto &[name, magic, type] : magic_names) {
    if (auto res =
            std::ranges::find_if(right_ports.begin(), right_ports.end(),
                                 [&name](auto &e) { return e.first == name; });
        res != right_ports.end()) {
      if (res->second->GetDataType() != type) {
        return absl::InvalidArgumentError(
            std::format("'{}' must be of type '{}' (found: '{}')!", magic, type,
                        res->second->GetDataType()));
      }
      out + Out::RIGHT / name + " = " + magic + ";" = 1;
    }
  }

  for (auto &res : out.parent.leftPorts) {
    out + Out::RIGHT / res.first + " = " + Out::LEFT / res.first + ";" = 1;
  }

  return out.GetStatus();
}

} // namespace msk::blender
