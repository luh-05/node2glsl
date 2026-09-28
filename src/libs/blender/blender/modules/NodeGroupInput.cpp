#include "blender/modules/modules.hpp"
#include <absl/status/status.h>
#include <absl/status/statusor.h>
#include <array>
#include <memory>
#include <mir/codegen.hpp>
#include <mir/node_graph/node_graph.hpp>

namespace msk::blender {

auto GenerateTokenStringNodeGroupInput(Out &out) -> absl::Status {
  constexpr std::pair<const char *, const char *> magic_names[] = {
      {"p3d_position0", "p3d_position"},
  };
  auto &right_ports = out.parent.rightPorts;
  for (auto &[name, magic] : magic_names) {
    if (auto res =
            std::ranges::find_if(right_ports.begin(), right_ports.end(),
                                 [&name](auto &e) { return e.first == name; });
        res != right_ports.end()) {
      out + Out::RIGHT / name + " = " + magic + ";" = 1;
    }
  }
  return out.GetStatus();
}

} // namespace msk::blender
