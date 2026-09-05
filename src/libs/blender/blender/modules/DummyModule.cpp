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
  uint32_t a;
  if (auto status = context->GetConstant<const uint32_t>(this, "a");
      !status.ok())
    return status.status();
  else
    a = status.value();

  out.AddLines("Demo Module: ", "beep boop... ");
  out.Add("Testing DSL");
  out.AddLine();

  out.AddLinesF({
      {"Constant 'a' is: {}", a},
      // {"bla {}", "f"},
      // {"Constant 'a' == 4: {}", a == 4 ? "true" : "false"},
      {false, "Left port 'value0' resolves to: "},
  });
  if (auto status = out.AddLeftPort(*this, "value0"); !status.ok()) {
    return status;
  }
  out.AddLine();

  bool b;
  if (auto status = context->GetConstant<const bool>(this, "b"); !status.ok())
    return status.status();
  else
    b = status.value();

  if (b) {
    out.AddLineF("Constant 'b' is {}!", "true");
  }

  return absl::OkStatus();
}
} // namespace msk::blender
