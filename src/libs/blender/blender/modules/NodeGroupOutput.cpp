#include "blender/modules/modules.hpp"
#include <absl/status/status.h>
#include <absl/status/statusor.h>
#include <array>
#include <memory>
#include <mir/codegen.hpp>
#include <mir/node_graph/node_graph.hpp>

namespace msk::blender {

auto GenerateTokenStringNodeGroupOutput(Out &out) -> absl::Status {
  constexpr std::tuple<const char *, const char *, const char *> magic_names[] =
      {
          {"p3d_sdf0", "p3d_sdf", "VALUE"},
          {"p3d_rgba0", "p3d_rgba", "COLOR"},
      };

  auto &left_ports = out.parent.leftPorts;

  for (auto &[name, magic, type] : magic_names) {
    if (auto res =
            std::ranges::find_if(left_ports.begin(), left_ports.end(),
                                 [&name](auto &e) { return e.first == name; });
        res != left_ports.end()) {
      if (res->second->GetDataType() != type) {
        return absl::InvalidArgumentError(
            std::format("'{}' must be of type '{}' (found: '{}')!", magic, type,
                        res->second->GetDataType()));
      }
      out + magic + " = " + Out::LEFT / name + ";" = 1;
    }
  }

  for (auto &res : left_ports | std::views::filter([](auto &e) {
                     return e.first.starts_with("mollusk_debug");
                   })) {
    out + res.first + " = " + Out::LEFT / res.first + ";" = 1;
  }

  return out.GetStatus();
}

} // namespace msk::blender
