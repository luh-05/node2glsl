#include "mir/node_graph/node_graph.hpp"
#include "modules.hpp"
#include <absl/status/status.h>
#include <absl/status/statusor.h>
#include <format>
#include <memory>
#include <mir/codegen.hpp>
#include <vector>

namespace msk::blender {
auto DummyModule::GenerateTokenString(ir::ContextProvider *context,
                                      ir::Module::Out out) -> absl::Status {
  auto a = context->GetConstant<const uint32_t>(this, "a");
  if (!a.ok()) {
    return absl::NotFoundError("Constant not found!");
  }

  *out = std::make_unique<ir::TextToken>(
      std::format("Constant 'a' is: {}\n", a.value()));
  *out = std::make_unique<ir::TextToken>("Left port 'value0' resolves to: ");
  *out = std::make_unique<ir::WildcardToken>(this->leftPorts["value0"].get());
  *out = std::make_unique<ir::TextToken>("\n");

  return absl::OkStatus();
}
} // namespace msk::blender
