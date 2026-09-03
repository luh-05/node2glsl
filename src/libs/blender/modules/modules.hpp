#include <memory>
#include <mir/node_graph/node_graph.hpp>
#pragma ONCE

namespace msk::blender {
/**
 *  @brief Dummy Module
 */
typedef class DummyModule : public ir::Module {
  auto GenerateTokenString(ir::ContextProvider context)
      -> absl::StatusOr<std::vector<std::unique_ptr<ir::CodegenToken>>>;
} DummyModule;
} // namespace msk::blender
