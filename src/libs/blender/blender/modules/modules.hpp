#include <absl/status/status.h>
#include <mir/node_graph/node_graph.hpp>
#pragma once

namespace msk::blender {

using Out = msk::ir::Module::Out;
/**
 *  @brief Dummy Module
 */
auto GenerateTokenStringDummy(Out &&out) -> absl::Status;
} // namespace msk::blender
