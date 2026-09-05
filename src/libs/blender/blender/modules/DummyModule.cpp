#include "modules.hpp"
#include <absl/status/status.h>
#include <absl/status/statusor.h>
#include <memory>
#include <mir/codegen.hpp>
#include <vector>

namespace msk::blender {
auto DummyModule::GenerateTokenString(ir::ContextProvider *context)
    -> absl::StatusOr<std::vector<std::unique_ptr<ir::CodegenToken>>> {
  auto v = std::vector<std::unique_ptr<ir::CodegenToken>>();

  v.push_back(std::make_unique<ir::TextToken>("Constant 'a' is: "));
  auto a = context->GetConstant<const char *>(this, "a");
  if (!a.ok()) {
    return absl::NotFoundError("Constant not found!");
  }
  v.push_back(std::make_unique<ir::TextToken>(std::string(a.value())));

  v.push_back(
      std::make_unique<ir::TextToken>("\nLeft port 'value0' resolves to: "));
  v.push_back(
      std::make_unique<ir::WildcardToken>(this->leftPorts["value0"].get()));

  return v;
}
} // namespace msk::blender
