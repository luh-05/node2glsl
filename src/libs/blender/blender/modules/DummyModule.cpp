#include "mir/node_graph/node_graph.hpp"
#include "modules.hpp"
#include <absl/status/status.h>
#include <absl/status/statusor.h>
#include <format>
#include <memory>
#include <mir/codegen.hpp>
#include <vector>

namespace msk::blender {
auto DummyModule::GenerateTokenString(ir::ContextProvider *context, Out out)
    -> absl::Status {
  // NICHT FINAL
  uint32_t a;
  if (auto status = context->GetConstant<const uint32_t>(this, "a");
      !status.ok())
    return status.status();
  else
    a = status.value();
  // -----------
  // NICHT FINAL
  bool b;
  if (auto status = context->GetConstant<const bool>(this, "b"); !status.ok())
    return status.status();
  else
    b = status.value();
  // -----------

  out + "Constant 'a' is: " + a = 1;
  out + "bla "
        "f" = 1;
  out + "Constant 'a' == 4: " + (a == 4) = 1;
  out + "Left port 'value0' resolves to: " + Out::Polarity::LEFT / "value0" = 1;
  out + "I'm not flushed!" = 2;

  out + "Constant 'b' is " + b = 1;

  return absl::OkStatus();
}
} // namespace msk::blender
