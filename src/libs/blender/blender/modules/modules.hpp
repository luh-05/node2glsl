#include <absl/status/status.h>
#include <memory>
#include <mir/node_graph/node_graph.hpp>
#pragma once

namespace msk::blender {
/**
 *  @brief Dummy Module
 */
typedef class DummyModule : public ir::Module {
public:
  auto GenerateTokenString(ir::ContextProvider *context, Out out)
      -> absl::Status;
  // auto GenerateTokenString(ir::ContextProvider *context)
  //     -> absl::StatusOr<std::vector<std::unique_ptr<ir::CodegenToken>>>;
} DummyModule;
} // namespace msk::blender
