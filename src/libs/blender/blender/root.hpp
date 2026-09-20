#pragma once

#include <mir/node_graph/node_graph.hpp>
#include <plugin_abi/plugin_abi.h>

namespace msk::blender {
template <msk::ir::Module::GenerateTokenString func>
void FuncWrapper(void *out, PluginStatus *status);
} // namespace msk::blender
