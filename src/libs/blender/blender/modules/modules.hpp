#include <absl/status/status.h>
#include <mir/node_graph/node_graph.hpp>
#pragma once

namespace msk::blender {
/**
 *  @brief Dummy Module
 */
class DummyModule : public ir::Module {
public:
  using Module::Module;
  auto GenerateTokenString(Out &&out) -> absl::Status;
};
} // namespace msk::blender
